#ifndef DRAWAREAFANCY_H
#define DRAWAREAFANCY_H

#include "DrawArea.h"

class DrawAreaFancy : public DrawArea
{
    Q_OBJECT

public:
    DrawAreaFancy(QWidget *parent = nullptr);
    ~DrawAreaFancy() = default;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
};

#endif // DRAWAREAFANCY_H
