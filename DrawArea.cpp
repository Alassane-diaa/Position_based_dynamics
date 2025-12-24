#include "DrawArea.h"
#include "Context.h"
#include <QPainter>
#include <QMouseEvent>
#include <iostream>

DrawArea::DrawArea(QWidget *parent)
    : QOpenGLWidget(parent)
    , context(new Context())
{
}

void DrawArea::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(this->rect(), QBrush(QColor(255, 255, 255)));
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