/**
 * /author Pawel Ptasznik
 */

#include "MainWindow.hpp"
#include "ui_mainwindow.h"

#include <memory>

#include <QMainWindow>
#include <QPixmap>
#include <QWidget>
#include <QDebug>

constexpr int FRAME_WIDTH = 960;
constexpr int FRAME_HEIGHT = 540;

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
  , ui_(new Ui::MainWindow)
  , frame_pixmap_(FRAME_WIDTH, FRAME_HEIGHT)
{
  ui_->setupUi(this);
  ui_->graphicsView->setScene(new QGraphicsScene(this));
  ui_->graphicsView->scene()->addPixmap(frame_pixmap_);
  auto items = ui_->graphicsView->scene()->items();
  qDebug() << "Num items: " << items.size();
}

MainWindow::~MainWindow() = default;
