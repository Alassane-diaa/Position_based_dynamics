#ifndef CLOTHSIMULATION_H
#define CLOTHSIMULATION_H

#include "DrawArea.h"
#include <vector>

class ClothSimulation : public DrawArea
{
    Q_OBJECT

public:
    ClothSimulation(QWidget *parent = nullptr);
    ~ClothSimulation() = default;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void animate() override;

private:
    void createCloth(const Vec2& topLeft, int rows, int cols, float spacing, float particleRadius, float mass, float stiffness);
    int gridRows = 0;
    int gridCols = 0;
    int draggedParticle = -1;
    std::vector<int> pinnedParticles;
};

#endif
