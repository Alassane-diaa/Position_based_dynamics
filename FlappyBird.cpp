#include "FlappyBird.h"
#include <QPainter>
#include <QMouseEvent>

FlappyBird::FlappyBird(QWidget *parent)
    : DrawArea(parent)
    , context(new ContextFB())
{
    context->getParticles().push_back(Bird{
        Vec2{-200.0f, 0.0f},       
        Vec2{-200.0f, 0.0f},       
        Vec2{0.0f, 0.0f},       
        15.0f,                 
        1.0f                   
    });

    context->getColliders().push_back(new PlanCollider(Vec2{0.0f, -250.0f}, Vec2{0.0f, 1.0f}));
    context->getColliders().push_back(new PlanCollider(Vec2{0.0f, 250.0f}, Vec2{0.0f, -1.0f}));
    
    context->getColliders().push_back(new PipeCollider(150.0f, 50.0f, 120.0f)); 
    context->getColliders().push_back(new PipeCollider(350.0f, -30.0f, 120.0f));
    context->getColliders().push_back(new PipeCollider(550.0f, 70.0f, 120.0f)); 

}

void FlappyBird::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(this->rect(), QBrush(QColor(135, 206, 235)));

    for (MyCollider* collider : context->getColliders()) {
        if (PipeCollider* pipe = dynamic_cast<PipeCollider*>(collider)) {
            float pipeX = pipe->getPosX();
            float gapY = pipe->getGapY();
            float gapSize = pipe->getGapSize();
            float half_width = PipeCollider::width / 2.0f;
            Point pipeCenter = worldToView(Vec2{pipeX, 0.0f});
            Point gapTopWorld = worldToView(Vec2{0.0f, gapY + gapSize / 2.0f});
            Point gapBottomWorld = worldToView(Vec2{0.0f, gapY - gapSize / 2.0f});
            
            p.setPen(QPen(QColor(0, 100, 0), 2));
            p.setBrush(QColor(46, 204, 113));
            QRectF topPipe(pipeCenter.x - half_width, 0, 
                          PipeCollider::width, gapTopWorld.y);
            p.drawRect(topPipe);
            QRectF bottomPipe(pipeCenter.x - half_width, gapBottomWorld.y,
                             PipeCollider::width, this->height() - gapBottomWorld.y);
            p.drawRect(bottomPipe);
        }
    }
    
    p.setBrush(QColor(139, 90, 43));
    Point groundPos = worldToView(Vec2{0.0f, -250.0f});
    p.drawRect(0, (int)groundPos.y, this->width(), 50);

    for (const Particle& particle : context->getParticles()) {
        Vec2 view_pos = worldToView(particle.predicted_pos);
        QPointF center(view_pos.x, view_pos.y);
        QRectF target(center.x() - particle.radius, center.y() - particle.radius,
                      particle.radius * 2, particle.radius * 2);
        // Pour le dessin de l'oiseau, j'ai utilisé de l'ia pour avoir un bon rendu
        p.setPen(QPen(QColor(243, 156, 18), 2));
        p.setBrush(QColor(241, 196, 15));  
        p.drawEllipse(target);        
        p.setBrush(Qt::white);
        p.drawEllipse(QPointF(center.x() + 5, center.y() - 3), 5, 5);
        p.setBrush(Qt::black);
        p.drawEllipse(QPointF(center.x() + 6, center.y() - 3), 2, 2);
    }
}

void FlappyBird::mousePressEvent(QMouseEvent* event)
{
    std::vector<Particle>& particles = context->getParticles();
    if (!particles.empty()) {
        Particle& bird = particles[0];
        bird.velocity.y = 500.0f; 
    }
}

void FlappyBird::mouseDoubleClickEvent(QMouseEvent* event)
{
    std::vector<Particle>& particles = context->getParticles();
    if (!particles.empty()) {
        Particle& bird = particles[0];
        bird.velocity.y = 500.0f; 
    }
}

void FlappyBird::animate()
{
    const float dt = 0.016f; 
    context->updatePhysicalSystem(dt);
    this->update(); 
}

