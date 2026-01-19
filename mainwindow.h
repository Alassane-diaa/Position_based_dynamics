#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QOpenGLFunctions>
#include <QTabWidget>
#include "DrawArea.h"
#include "DrawAreaFancy.h"
#include "FlappyBird.h"
#include "ClothSimulation.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DrawArea* draw_area;
    DrawAreaFancy* draw_area_fancy;
    FlappyBird* flappy_bird;
    ClothSimulation* cloth_sim;

};
#endif // MAINWINDOW_H
