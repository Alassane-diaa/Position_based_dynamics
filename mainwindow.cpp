#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , draw_area(new DrawArea(this))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(draw_area);
    
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, draw_area, qOverload<>(&DrawArea::animate));
    timer->start(16);

}



MainWindow::~MainWindow()
{
    delete ui;
}
