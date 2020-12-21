/**
 * /author Pawel Ptasznik
 */

#include "MainWindow.hpp"
#include "ui_mainwindow.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <memory>

#include <QDebug>
#include <QGraphicsItem>
#include <QImage>
#include <QMainWindow>
#include <QObject>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

constexpr int FRAME_WIDTH = 960;
constexpr int FRAME_HEIGHT = 540;

MainWindow::MainWindow(std::shared_ptr<AsyncFrameListener> listener, QWidget* parent): QMainWindow(parent)
  , ui_(new Ui::MainWindow)
  , frame_pixmap_(FRAME_WIDTH, FRAME_HEIGHT)
  , listener_(listener)
{
  ui_->setupUi(this);
  ui_->graphicsView->setScene(new QGraphicsScene(this));
  ui_->graphicsView->scene()->addPixmap(frame_pixmap_);
  auto items = ui_->graphicsView->scene()->items();
  qDebug() << "Num items: " << items.size();
  QObject::connect(ui_->startPushButton, &QPushButton::clicked, [this]() {
    listener_->setOnFrameCallback(std::bind(&MainWindow::frameCallback, this, std::placeholders::_1));
    listener_->start();
  });
  QObject::connect(ui_->stopPushButton, &QPushButton::clicked, [this]() { listener_->stop(); });
  QObject::connect(this, &MainWindow::frameReady, this, &MainWindow::refreshFrame);
}

MainWindow::~MainWindow()
{
  listener_->stop();
}

void MainWindow::refreshFrame()
{
  auto pixmap_item = qgraphicsitem_cast<QGraphicsPixmapItem*>(
    ui_->graphicsView->scene()->items().first());
  pixmap_item->setPixmap(frame_pixmap_);
}

void MainWindow::frameCallback(const cv::Mat& frame)
{
  cv::Mat resized;
  cv::resize(frame, resized, cv::Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, cv::INTER_CUBIC);
  QImage im(resized.data, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH, QImage::Format_Grayscale8);
  frame_pixmap_ = QPixmap::fromImage(im);
  emit frameReady();
}

