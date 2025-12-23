#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QOpenGLWidget>

class DrawArea : public QOpenGLWidget
{
    Q_OBJECT

public:
    DrawArea(QWidget *parent = nullptr);
    ~DrawArea() = default;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void animate();
    
private:
    std::vector<QPointF> points;

};

#endif // DRAWAREA_H