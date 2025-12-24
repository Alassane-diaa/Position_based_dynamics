#include "DrawArea.h"
#include "Context.h"
#include <QPainter>
#include <QMouseEvent>
#include <iostream>

DrawArea::DrawArea(QWidget *parent)
    : QOpenGLWidget(parent)
    , context(new Context())
{
    std::vector<MyCollider*>& colliders = context->getColliders();
    PlanCollider* wall1 = new PlanCollider(Vec2{-280.0f, 0.0f}, Vec2{1.0f, 0.0f});
    PlanCollider* wall2 = new PlanCollider(Vec2{280.0f, 0.0f}, Vec2{-1.0f, 0.0f});
    PlanCollider* ground = new PlanCollider(Vec2{0.0f, -280.0f}, Vec2{0.0f, 1.0f});
    PlanCollider* pente = new PlanCollider(Vec2{-200.0f, -100.0f}, Vec2{1.0f, 1.0f});
    SphereCollider* ball = new SphereCollider(Vec2{100.0f, 0.0f}, 50.0f);
    colliders.push_back(wall1);
    colliders.push_back(wall2);
    colliders.push_back(ground);
    colliders.push_back(pente);
    colliders.push_back(ball);
}

void DrawArea::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(this->rect(), QBrush(QColor(255, 255, 255)));
    for (const MyCollider* collider : context->getColliders()) {
        if (const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(collider)) {
            Point center = worldToView(sphere->getSphereCenter());
            QRectF target(center.x - sphere->getSphereRadius(), center.y - sphere->getSphereRadius(),
                          sphere->getSphereRadius() * 2, sphere->getSphereRadius() * 2);
            p.setPen(Qt::NoPen);
            p.setBrush(Qt::red);
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
            p.setBrush(Qt::black);
            p.drawPolygon(forbidden);
        }
    }
    for (const Particle& particle : context->getParticles()) {
        Vec2 view_pos = worldToView(particle.predicted_pos);
        QPointF center(view_pos.x, view_pos.y);
        QRectF target(center.x() - particle.radius, center.y() - particle.radius,
                      particle.radius * 2, particle.radius * 2);
        p.setPen(Qt::blue);
        p.setBrush(Qt::green);
        p.drawEllipse(target);
    }
}

void DrawArea::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF point = event->pos();
    Vec2 world_pos = viewToWorld(Vec2{(float)point.x(), (float)point.y()});
    context->getParticles().push_back(Particle{world_pos, world_pos, Vec2{0, 0}, 20.0f, 5.0f});
    update();
}

Point DrawArea::worldToView(const Point& world_pos)
{
    Point view_pos;
    view_pos.x = world_pos.x + this->width() / 2.0f;
    view_pos.y = - (world_pos.y - this->height() / 2.0f);
    return view_pos;
}

Point DrawArea::viewToWorld(const Point& view_pos)
{
    Point world_pos;
    world_pos.x = view_pos.x - this->width() / 2.0f;
    world_pos.y = - (view_pos.y - this->height() / 2.0f);
    return world_pos;
}

void DrawArea::animate()
{
    if (context->getParticles().empty()) return;
    context->updatePhysicalSystem(0.016f);
    update();
}