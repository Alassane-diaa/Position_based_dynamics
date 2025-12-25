#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "DrawArea.h"

class FlappyBird : public DrawArea
{
    Q_OBJECT

public:
    FlappyBird(QWidget *parent = nullptr);
    ~FlappyBird() = default;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    Context* context;

};


#endif // FLAPPYBIRD_H