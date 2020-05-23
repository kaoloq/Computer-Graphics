#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->scene_widget->setFocusPolicy(Qt::StrongFocus);
    ui->scene_widget->setFocus(Qt::ActiveWindowFocusReason);
}

MainWindow::~MainWindow()
{
    delete ui;
}

