#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QOpenGLWidget>
#include "Context.h"

enum class SpawnMode {
    SingleParticle,
    LinkedStructure
};

class DrawArea : public QOpenGLWidget
{
    Q_OBJECT

public:
    DrawArea(QWidget *parent = nullptr);
    virtual ~DrawArea() = default;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    virtual void animate();
    Point worldToView(const Point& world_pos);
    Point viewToWorld(const Point& view_pos);
    void toggleSpawnMode();  // Basculer entre les modes
    SpawnMode getSpawnMode() const { return spawnMode; }
    
protected:
    Context* context;
    SpawnMode spawnMode = SpawnMode::SingleParticle;  // Mode par défaut
};

#endif // DRAWAREA_H