#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , draw_area(new DrawArea(this))
    , flappy_bird(new FlappyBird(this))
{
    ui->setupUi(this);
    ui->tabWidget->addTab(draw_area, "Simple simulation");
    ui->tabWidget->addTab(flappy_bird, "Flappy Bird");
    
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, draw_area, qOverload<>(&DrawArea::animate));
    QObject::connect(timer, &QTimer::timeout, flappy_bird, qOverload<>(&FlappyBird::animate));
    timer->start(16);
}



MainWindow::~MainWindow()
{
    delete ui;
}
