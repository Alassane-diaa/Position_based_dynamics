#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , draw_area(new DrawArea(this))
    , draw_area_fancy(new DrawAreaFancy(this))
    , flappy_bird(new FlappyBird(this))
    , cloth_sim(new ClothSimulation(this))
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    ui->tabWidget->addTab(draw_area, "Simple simulation");
    ui->tabWidget->addTab(draw_area_fancy, "Fancy simulation");
    ui->tabWidget->addTab(flappy_bird, "Flappy Bird");
    ui->tabWidget->addTab(cloth_sim, "Cloth");

    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, draw_area, qOverload<>(&DrawArea::animate));
    QObject::connect(timer, &QTimer::timeout, draw_area_fancy, qOverload<>(&DrawAreaFancy::animate));
    QObject::connect(timer, &QTimer::timeout, flappy_bird, qOverload<>(&FlappyBird::animate));
    QObject::connect(timer, &QTimer::timeout, cloth_sim, qOverload<>(&ClothSimulation::animate));
    timer->start(16);
}



MainWindow::~MainWindow()
{
    delete ui;
}
