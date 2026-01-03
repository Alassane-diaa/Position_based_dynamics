#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QOpenGLWidget>
#include "Context.h"


class DrawArea : public QOpenGLWidget
{
    Q_OBJECT

public:
    DrawArea(QWidget *parent = nullptr);
    virtual ~DrawArea() = default;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void animate();
    Point worldToView(const Point& world_pos);
    Point viewToWorld(const Point& view_pos);
    
protected:
    Context* context;
};

#endif // DRAWAREA_H