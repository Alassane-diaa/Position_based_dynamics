#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "DrawArea.h"
#include "Context.h"

using Bird = Particle;

class FlappyBird : public DrawArea
{
    Q_OBJECT

public:
    FlappyBird(QWidget *parent = nullptr);
    ~FlappyBird() = default;
    void animate();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    Context* context;
    bool isOn = false;
    void movePipes(float dt);

};


#endif // FLAPPYBIRD_H