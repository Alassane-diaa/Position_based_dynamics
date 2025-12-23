#include "DrawArea.h"
#include <QPainter>
#include <QMouseEvent>

DrawArea::DrawArea(QWidget *parent)
    : QOpenGLWidget(parent)
{
}

void DrawArea::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(this->rect(), QBrush(QColor(255, 255, 255)));
    qreal size = this->width()/4;
    for (const QPointF& point : points) {
        QRectF target(point.x() - size/2, point.y() - size/2, size, size);
        p.setPen(Qt::blue);
        p.setBrush(Qt::green);
        p.drawEllipse(target);
    }
}

void DrawArea::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF point = event->pos();
    points.push_back(point);
    update();
}

void DrawArea::animate()
{
    if (points.empty()) return;
    qreal size = this->width()/4;
    for (QPointF& point : points) {
        if (point.y() < this->height() - size/2) {
            point.ry() += 1.0;
        } else {
            point.ry() = this->height() - size/2;
        }
    }
    update();
}