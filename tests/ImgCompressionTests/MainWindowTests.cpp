/**
 * /author Pawel Ptasznik
 */

#include "MainWindow.hpp"
#include "AsyncFrameListenerMock.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

class MainWindowFixture: public ::testing::Test
{
 protected:
  MainWindowFixture()
  {
    listenerMock_.reset(new AsyncFrameListenerMock);
    sut_.reset(new MainWindow(listenerMock_));
  }

  std::shared_ptr<AsyncFrameListenerMock> listenerMock_;
  std::unique_ptr<MainWindow> sut_;
};

TEST_F(MainWindowFixture, startStopToggleListenToFrames)
{
}

TEST_F(MainWindowFixture, requestRoiWhenItIsSelected)
{
}

TEST_F(MainWindowFixture, displayFrameWhenItArrives)
{
}
