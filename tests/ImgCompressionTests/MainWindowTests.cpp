/**
 * /author Pawel Ptasznik
 */

#include "MainWindow.hpp"
#include "AsyncFrameListenerMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <opencv2/core.hpp>

#include <QGraphicsItem>
#include <QPushButton>
#include <QTest>
#include <QGraphicsView>

#include <memory>

constexpr int TIMEOUT = 1000;

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::DoDefault;
using testing::NiceMock;

class MainWindowFixture: public ::testing::Test
{
 protected:
  MainWindowFixture()
  {
    listenerMock_.reset(new NiceMock<AsyncFrameListenerMock>);
    callback_ = nullptr;
    ON_CALL(*listenerMock_, setOnFrameCallback(_)).WillByDefault(
      Invoke([this](AsyncFrameListener::Callback cb) { callback_ = cb; }));
    ON_CALL(*listenerMock_, start()).WillByDefault(Return(true));
    ON_CALL(*listenerMock_, stop()).WillByDefault(Return(true));
    sut_.reset(new MainWindow(listenerMock_));
  }

  std::shared_ptr<NiceMock<AsyncFrameListenerMock> > listenerMock_;
  AsyncFrameListener::Callback callback_;
  std::unique_ptr<MainWindow> sut_;
};

TEST_F(MainWindowFixture, startStopToggleListenToFrames)
{
  auto startPushButton = sut_->findChild<QPushButton*>("startPushButton");
  auto stopPushButton = sut_->findChild<QPushButton*>("stopPushButton");
  auto graphicsView = sut_->findChild<QGraphicsView*>("graphicsView");
  EXPECT_CALL(*listenerMock_, start()).WillOnce(DoDefault());
  EXPECT_CALL(*listenerMock_, stop()).WillOnce(DoDefault());
  QTest::mouseClick(startPushButton, Qt::LeftButton);
  ASSERT_TRUE(QTest::qWaitFor([this]() { return callback_ != nullptr; }, TIMEOUT));
  cv::Mat frame(20, 15, CV_8UC1, cv::Scalar(69));
  callback_(frame);
  auto image = qgraphicsitem_cast<QGraphicsPixmapItem*>(
    graphicsView->scene()->items().first())->pixmap().toImage();
  ASSERT_TRUE(image.isGrayscale());
  ASSERT_EQ(960*540, image.width() * image.height());
}

TEST_F(MainWindowFixture, requestRoiWhenItIsSelected)
{
}

TEST_F(MainWindowFixture, displayFrameWhenItArrives)
{
}
