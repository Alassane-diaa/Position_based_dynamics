#include "FlappyBird.h"

FlappyBird::FlappyBird(QWidget *parent)
    : DrawArea(parent)
    , context(new Context())
{
}

void FlappyBird::paintEvent(QPaintEvent *event)
{
    DrawArea::paintEvent(event);
}

void FlappyBird::mouseDoubleClickEvent(QMouseEvent* event)
{}

