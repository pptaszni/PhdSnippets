/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "AsyncFrameListener.hpp"

#include <memory>

#include <opencv2/core.hpp>

#include <QMainWindow>
#include <QPixmap>
#include <QWidget>

namespace Ui
{
class MainWindow;
}  // namespace Ui


class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow(std::shared_ptr<AsyncFrameListener> listener, QWidget* parent = nullptr);
  ~MainWindow();

signals:
  void frameReady();

private slots:
  void refreshFrame();

private:
  void frameCallback(const cv::Mat& frame);
  std::unique_ptr<Ui::MainWindow> ui_;
  QPixmap frame_pixmap_;
  std::shared_ptr<AsyncFrameListener> listener_;
};
