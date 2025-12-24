#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QOpenGLWidget>
#include "Context.h"

using Point = Vec2;


class DrawArea : public QOpenGLWidget
{
    Q_OBJECT

public:
    DrawArea(QWidget *parent = nullptr);
    ~DrawArea() = default;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void animate();
    Point worldToView(const Point& world_pos);
    Point viewToWorld(const Point& view_pos);
    
    
private:
    Context* context;
};

#endif // DRAWAREA_H