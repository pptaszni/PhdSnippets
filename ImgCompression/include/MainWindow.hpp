/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <memory>

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
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private:
  std::unique_ptr<Ui::MainWindow> ui_;
  QPixmap frame_pixmap_;
};
