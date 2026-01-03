#include "DrawAreaFancy.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <cstdlib>
#include <ctime>

DrawAreaFancy::DrawAreaFancy(QWidget *parent)
    : DrawArea(parent)
{
    context->getParticles().clear();
    context->getColliders().clear();

    std::srand(std::time(nullptr));

    std::vector<MyCollider*>& colliders = context->getColliders();
    
    colliders.push_back(new PlanCollider(Vec2{-380.0f, 0.0f}, Vec2{1.0f, 0.0f}));
    colliders.push_back(new PlanCollider(Vec2{380.0f, 0.0f}, Vec2{-1.0f, 0.0f}));
    colliders.push_back(new PlanCollider(Vec2{0.0f, -260.0f}, Vec2{0.0f, 1.0f}));
    
    colliders.push_back(new BarCollider(Vec2{-300.0f, 180.0f}, Vec2{-100.0f, 140.0f}, 10.0f));
    colliders.push_back(new BarCollider(Vec2{100.0f, 140.0f}, Vec2{300.0f, 180.0f}, 10.0f));
    
    colliders.push_back(new SphereCollider(Vec2{0.0f, 100.0f}, 35.0f));
    
    colliders.push_back(new BarCollider(Vec2{-250.0f, 20.0f}, Vec2{-50.0f, -20.0f}, 10.0f));
    colliders.push_back(new BarCollider(Vec2{50.0f, -20.0f}, Vec2{250.0f, 20.0f}, 10.0f));
    
    colliders.push_back(new SphereCollider(Vec2{-180.0f, -80.0f}, 25.0f));
    colliders.push_back(new SphereCollider(Vec2{180.0f, -80.0f}, 25.0f));
    
    colliders.push_back(new BarCollider(Vec2{-100.0f, -150.0f}, Vec2{100.0f, -150.0f}, 10.0f));
    
    colliders.push_back(new SphereCollider(Vec2{-300.0f, -180.0f}, 20.0f));
    colliders.push_back(new SphereCollider(Vec2{300.0f, -180.0f}, 20.0f));

    std::vector<Particle>& particles = context->getParticles();
    particles.push_back(Particle{Vec2{-200.0f, 280.0f}, Vec2{-200.0f, 280.0f}, Vec2{50.0f, 0.0f}, 15.0f, 6.0f});
    particles.push_back(Particle{Vec2{0.0f, 290.0f}, Vec2{0.0f, 290.0f}, Vec2{0.0f, 0.0f}, 18.0f, 8.0f});
    particles.push_back(Particle{Vec2{200.0f, 280.0f}, Vec2{200.0f, 280.0f}, Vec2{-50.0f, 0.0f}, 15.0f, 6.0f});
}

void DrawAreaFancy::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    
    QLinearGradient gradient(0, 0, 0, this->height());
    gradient.setColorAt(0, QColor(135, 206, 250));
    gradient.setColorAt(1, QColor(70, 130, 180));
    p.fillRect(this->rect(), gradient);
    
    QColor rampColor(139, 90, 43);
    QColor sphereColor(100, 100, 100);
    QColor wallColor(60, 60, 60);
    
    for (const MyCollider* collider : context->getColliders()) {
        if (const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(collider)) {
            Point center = worldToView(sphere->getSphereCenter());
            float r = sphere->getSphereRadius();
            QRectF target(center.x - r, center.y - r, r * 2, r * 2);
            
            QRadialGradient sphereGrad(center.x - r/3, center.y - r/3, r * 1.5f);
            sphereGrad.setColorAt(0, QColor(180, 180, 180));
            sphereGrad.setColorAt(0.5, sphereColor);
            sphereGrad.setColorAt(1, QColor(40, 40, 40));
            
            p.setPen(QPen(QColor(30, 30, 30), 2));
            p.setBrush(sphereGrad);
            p.drawEllipse(target);
            
        } else if (const PlanCollider* plane = dynamic_cast<const PlanCollider*>(collider)) {
            Point point = worldToView(plane->getPointOnPlane());
            Vec2 normal = plane->getPlaneNormal();
            normal.y = -normal.y;
            QPolygonF forbidden;
            float p1x = point.x - 1000 * normal.y;
            float p1y = point.y + 1000 * normal.x;
            float p2x = point.x + 1000 * normal.y;
            float p2y = point.y - 1000 * normal.x;
            forbidden << QPointF(p1x, p1y)
                      << QPointF(p2x, p2y)
                      << QPointF(p2x - 1000 * normal.x, p2y - 1000 * normal.y)
                      << QPointF(p1x - 1000 * normal.x, p1y - 1000 * normal.y);
            p.setPen(Qt::NoPen);
            p.setBrush(wallColor);
            p.drawPolygon(forbidden);
            
        } else if (const BarCollider* bar = dynamic_cast<const BarCollider*>(collider)) {
            Point start = worldToView(bar->getStartPoint());
            Point end = worldToView(bar->getEndPoint());
            Vec2 barVector;
            barVector.x = end.x - start.x;
            barVector.y = end.y - start.y;
            float length = std::sqrt(barVector.x * barVector.x + barVector.y * barVector.y);
            Vec2 unitPerp;
            unitPerp.x = -barVector.y / length;
            unitPerp.y = barVector.x / length;
            float half_thickness = bar->getBarThickness() / 2.0f;

            QPolygonF rectangle;
            rectangle << QPointF(start.x + unitPerp.x * half_thickness, start.y + unitPerp.y * half_thickness)
                      << QPointF(end.x + unitPerp.x * half_thickness, end.y + unitPerp.y * half_thickness)
                      << QPointF(end.x - unitPerp.x * half_thickness, end.y - unitPerp.y * half_thickness)
                      << QPointF(start.x - unitPerp.x * half_thickness, start.y - unitPerp.y * half_thickness);
            
            p.setPen(QPen(QColor(90, 60, 30), 2));
            p.setBrush(rampColor);
            p.drawPolygon(rectangle);
        }
    }
    
    QColor particleColors[] = {
        QColor(231, 76, 60),
        QColor(230, 126, 34),
        QColor(241, 196, 15),
        QColor(46, 204, 113),
        QColor(52, 152, 219),
        QColor(155, 89, 182)
    };
    
    int colorIndex = 0;
    for (const Particle& particle : context->getParticles()) {
        Vec2 view_pos = worldToView(particle.predicted_pos);
        QPointF center(view_pos.x, view_pos.y);
        float r = particle.radius;
        QRectF target(center.x() - r, center.y() - r, r * 2, r * 2);
        
        QColor baseColor = particleColors[colorIndex % 6];
        
        QRadialGradient ballGrad(center.x() - r/3, center.y() - r/3, r * 1.5f);
        ballGrad.setColorAt(0, baseColor.lighter(150));
        ballGrad.setColorAt(0.5, baseColor);
        ballGrad.setColorAt(1, baseColor.darker(200));
        
        p.setPen(QPen(baseColor.darker(150), 2));
        p.setBrush(ballGrad);
        p.drawEllipse(target);
        
        colorIndex++;
    }
}

void DrawAreaFancy::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF point = event->pos();
    Vec2 world_pos = viewToWorld(Vec2{(float)point.x(), (float)point.y()});
    float radius = 10.0f + (std::rand() % 15);
    float mass = radius / 3.0f;
    context->getParticles().push_back(Particle{world_pos, world_pos, Vec2{0, 0}, radius, mass});
    update();
}
