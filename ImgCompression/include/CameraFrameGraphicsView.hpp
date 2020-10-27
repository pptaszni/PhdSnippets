/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <memory>

#include <QGraphicsView>
#include <QMouseEvent>
#include <QRect>
#include <QWidget>

class CameraFrameGraphicsView : public QGraphicsView
{
  Q_OBJECT
public:
  explicit CameraFrameGraphicsView(QWidget* parent);
  ~CameraFrameGraphicsView() override = default;

signals:
  void leftMousePressed(int x, int y);
  void focusSelected(QRect selction);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  QPoint origin_;
};
