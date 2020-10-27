/**
 * /author Pawel Ptasznik
 */

#include <memory>

#include <QDebug>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QRect>
#include <QWidget>

#include "CameraFrameGraphicsView.hpp"

CameraFrameGraphicsView::CameraFrameGraphicsView(QWidget* parent): QGraphicsView(parent)
  , origin_()
{
}

void CameraFrameGraphicsView::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    auto p = mapToScene(event->pos()).toPoint();
    origin_ = p;
    qDebug() << "mousePressEvent[" << event->x() << ", " << event->y() << "]";
    qDebug() << "mapped[" << p.x() << ", " << p.y() << "]";
    emit leftMousePressed(p.x(), p.y());
  }
  QGraphicsView::mousePressEvent(event);
}

void CameraFrameGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    auto p = mapToScene(event->pos()).toPoint();
    if (!(p.x() == origin_.x() && p.y() == origin_.y()))
    {
      QRect selection(origin_, p);
      qDebug() << "new selection: " << selection;
      emit focusSelected(selection);
    }
  }
  QGraphicsView::mouseReleaseEvent(event);
}
