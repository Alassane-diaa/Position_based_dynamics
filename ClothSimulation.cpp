#include "ClothSimulation.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>

ClothSimulation::ClothSimulation(QWidget *parent)
    : DrawArea(parent)
{
    context->getParticles().clear();
    context->getColliders().clear();
    context->getParticleLinks().clear();

    std::vector<MyCollider*>& colliders = context->getColliders();
    colliders.push_back(new PlanCollider(Vec2{-380.0f, 0.0f}, Vec2{1.0f, 0.0f}));
    colliders.push_back(new PlanCollider(Vec2{380.0f, 0.0f}, Vec2{-1.0f, 0.0f}));
    colliders.push_back(new PlanCollider(Vec2{0.0f, -260.0f}, Vec2{0.0f, 1.0f}));
    colliders.push_back(new SphereCollider(Vec2{0.0f, -70.0f}, 80.0f));

    createCloth(Vec2{-150.0f, 240.0f}, 10, 12, 25.0f, 5.0f, 1.0f, 0.95f);

    for (int i = 0; i < gridCols; ++i) {
        pinnedParticles.push_back(i);
    }
}

void ClothSimulation::createCloth(const Vec2& topLeft, int rows, int cols, float spacing, float particleRadius, float mass, float stiffness)
{
    gridRows = rows;
    gridCols = cols;
    int startIndex = context->getParticles().size();
    std::vector<Particle>& particles = context->getParticles();
    std::vector<ParticleLink>& links = context->getParticleLinks();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            Vec2 pos = {topLeft.x + col * spacing, topLeft.y - row * spacing};
            particles.push_back(Particle{pos, pos, Vec2{0, 0}, particleRadius, mass});
        }
    }

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int current = startIndex + row * cols + col;
            
            if (col < cols - 1) {
                int right = current + 1;
                links.push_back(ParticleLink{current, right, spacing, stiffness});
            }
            
            if (row < rows - 1) {
                int below = current + cols;
                links.push_back(ParticleLink{current, below, spacing, stiffness});
            }
            
            if (col < cols - 1 && row < rows - 1) {
                int diagRight = current + cols + 1;
                float diagLength = spacing * std::sqrt(2.0f);
                links.push_back(ParticleLink{current, diagRight, diagLength, stiffness * 0.5f});
            }
            
            if (col > 0 && row < rows - 1) {
                int diagLeft = current + cols - 1;
                float diagLength = spacing * std::sqrt(2.0f);
                links.push_back(ParticleLink{current, diagLeft, diagLength, stiffness * 0.5f});
            }
        }
    }
}

void ClothSimulation::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(this->rect(), QColor(30, 30, 40));

    for (const MyCollider* collider : context->getColliders()) {
        if (const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(collider)) {
            Point center = worldToView(sphere->getSphereCenter());
            float r = sphere->getSphereRadius();
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(80, 80, 100));
            p.drawEllipse(QPointF(center.x, center.y), r, r);
        }
    }

    const std::vector<Particle>& particles = context->getParticles();
    
    for (const ParticleLink& link : context->getParticleLinks()) {
        if (link.particleA < 0 || link.particleA >= (int)particles.size() ||
            link.particleB < 0 || link.particleB >= (int)particles.size()) continue;
        
        Vec2 posA = worldToView(particles[link.particleA].predicted_pos);
        Vec2 posB = worldToView(particles[link.particleB].predicted_pos);
        
        float dx = posB.x - posA.x;
        float dy = posB.y - posA.y;
        float dist = std::sqrt(dx*dx + dy*dy);
        float stretch = dist / (link.restLength + 1.0f);
        
        int r = std::min(255, (int)(100 + stretch * 100));
        int g = std::max(0, 200 - (int)(stretch * 150));
        int b = 150;
        
        p.setPen(QPen(QColor(r, g, b), 2));
        p.drawLine(QPointF(posA.x, posA.y), QPointF(posB.x, posB.y));
    }

    for (size_t i = 0; i < particles.size(); ++i) {
        Vec2 view_pos = worldToView(particles[i].predicted_pos);
        float r = particles[i].radius;
        
        bool isPinned = std::find(pinnedParticles.begin(), pinnedParticles.end(), (int)i) != pinnedParticles.end();
        
        if (isPinned) {
            p.setBrush(QColor(255, 100, 100));
        } else {
            p.setBrush(QColor(200, 200, 255));
        }
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(view_pos.x, view_pos.y), r, r);
    }
    
    p.setPen(Qt::white);
    p.drawText(10, 20, "Clic : attraper | Double-clic : fixer/defixer");
}

void ClothSimulation::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF point = event->pos();
    Vec2 world_pos = viewToWorld(Vec2{(float)point.x(), (float)point.y()});
    
    std::vector<Particle>& particles = context->getParticles();
    
    for (size_t i = 0; i < particles.size(); ++i) {
        float dx = particles[i].pos.x - world_pos.x;
        float dy = particles[i].pos.y - world_pos.y;
        float dist = std::sqrt(dx*dx + dy*dy);
        
        if (dist < 20.0f) {
            auto it = std::find(pinnedParticles.begin(), pinnedParticles.end(), (int)i);
            if (it != pinnedParticles.end()) {
                pinnedParticles.erase(it);
            } else {
                pinnedParticles.push_back(i);
            }
            break;
        }
    }
    update();
}

void ClothSimulation::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF point = event->pos();
        Vec2 world_pos = viewToWorld(Vec2{(float)point.x(), (float)point.y()});
        
        std::vector<Particle>& particles = context->getParticles();
        for (size_t i = 0; i < particles.size(); ++i) {
            float dx = particles[i].pos.x - world_pos.x;
            float dy = particles[i].pos.y - world_pos.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            if (dist < 20.0f) {
                draggedParticle = i;
                break;
            }
        }
    }
}

void ClothSimulation::mouseReleaseEvent(QMouseEvent* event)
{
    draggedParticle = -1;
}

void ClothSimulation::mouseMoveEvent(QMouseEvent* event)
{
    if (draggedParticle >= 0 && draggedParticle < (int)context->getParticles().size()) {
        QPointF point = event->pos();
        Vec2 world_pos = viewToWorld(Vec2{(float)point.x(), (float)point.y()});
        
        Particle& part = context->getParticles()[draggedParticle];
        part.pos = world_pos;
        part.predicted_pos = world_pos;
        part.velocity = Vec2{0, 0};
    }
    update();
}

void ClothSimulation::animate()
{
    if (context->getParticles().empty()) return;
    
    std::vector<Particle>& particles = context->getParticles();
    std::vector<Vec2> savedPositions(pinnedParticles.size());
    
    for (size_t i = 0; i < pinnedParticles.size(); ++i) {
        int idx = pinnedParticles[i];
        if (idx >= 0 && idx < (int)particles.size()) {
            savedPositions[i] = particles[idx].pos;
        }
    }
    
    context->updatePhysicalSystem(0.016f);
    
    for (size_t i = 0; i < pinnedParticles.size(); ++i) {
        int idx = pinnedParticles[i];
        if (idx >= 0 && idx < (int)particles.size()) {
            particles[idx].pos = savedPositions[i];
            particles[idx].predicted_pos = savedPositions[i];
            particles[idx].velocity = Vec2{0, 0};
        }
    }
    
    update();
}
