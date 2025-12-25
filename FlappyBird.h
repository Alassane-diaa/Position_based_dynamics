#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "DrawArea.h"
#include "ContextFB.h"

using Bird = Particle;

class FlappyBird : public DrawArea
{
    Q_OBJECT

public:
    FlappyBird(QWidget *parent = nullptr);
    ~FlappyBird() = default;
    void paintEvent(QPaintEvent *event) override;
    void animate();
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    ContextFB* context;

};


#endif // FLAPPYBIRD_H