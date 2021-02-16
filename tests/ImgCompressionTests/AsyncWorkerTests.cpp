/**
 * /author Pawel Ptasznik
 */

#include "AsyncFrameListener.hpp"
#include "NetworkClientMock.hpp"
#include "Logger.hpp"
#include "CompressionAlgorithmTypes.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <future>
#include <memory>
#include <mutex>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;

constexpr size_t MAX_PAYLOAD = 200;
constexpr auto TEST_TIMEOUT = std::chrono::seconds(1);

class AsyncWorkerFixture: public ::testing::Test
{
 protected:
  AsyncWorkerFixture(): logger_("AsyncWorkerFixture")
  {
    networkMock_.reset(new NiceMock<NetworkClientMock>);
    ON_CALL(*networkMock_, connect(_)).WillByDefault(Return(true));
    // TODO: for now only testing no compression scenario to be able to compare frames
    sut_.reset(new AsyncFrameListener(networkMock_, NONE));
  }

  Logger logger_;
  std::shared_ptr<NiceMock<NetworkClientMock> > networkMock_;
  std::unique_ptr<AsyncFrameListener> sut_;
};

TEST_F(AsyncWorkerFixture, startStopOnlyOnceTest)
{
  ASSERT_TRUE(sut_->start());
  ASSERT_FALSE(sut_->start());
  ASSERT_TRUE(sut_->stop());
  ASSERT_FALSE(sut_->stop());
}

TEST_F(AsyncWorkerFixture, failToStartIfFailToConnect)
{
  EXPECT_CALL(*networkMock_, connect(_)).WillOnce(Return(false));
  ASSERT_FALSE(sut_->start());
}

TEST_F(AsyncWorkerFixture, sendCorrectRoiOverNetwork)
{
  // ASSERT_TRUE(sut_->start());
  ROI expectedRoi{4, 3, 2, 1};
  std::unique_ptr<uint8_t[]> buff;
  EXPECT_CALL(*networkMock_, send(_, _)).WillOnce(Invoke([this, &buff](const uint8_t* data, size_t length)
    {
      logger_->debug("send: going to copy data of size: {}", length);
      buff.reset(new uint8_t[length]);
      std::memcpy(buff.get(), data, length);
      return length;
    }));
  ROI result{};
  sut_->requestRoi(expectedRoi);
  ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(networkMock_.get()));
  std::memcpy(&result, buff.get(), sizeof(ROI));
  ASSERT_EQ(expectedRoi, result);
}

bool areEqual(const cv::Mat a, const cv::Mat b)
{
  if (a.total() != b.total()) return false;
  return std::equal(a.begin<uchar>(), a.end<uchar>(), b.begin<uchar>());
}

// TODO: testeed just this simple scenario, maybe later test something more
TEST_F(AsyncWorkerFixture, callbackWithCorrectFrame)
{
  cv::Mat frame(64, 48, CV_8UC1);
  size_t framesize = frame.total();
  std::generate(frame.data, frame.data+framesize, [n = 0]() mutable { return n++; });
  std::condition_variable cvar;
  EXPECT_CALL(*networkMock_, receive(_, _)).WillRepeatedly(Invoke([this, frame](uint8_t* data, size_t length)
  {
    if (length == sizeof(MsgHeader))
    {
      MsgHeader h{ (uint16_t)frame.total(), (uint16_t)frame.cols, (uint16_t)frame.rows, 0 };
      h.crc = h.calculateCrc();
      std::memcpy(data, &h, length);
      return length;
    }
    logger_->debug("receive: going to copy data of size: {}", length);
    std::memcpy(data, frame.data, length);
    return length;
  }));
  cv::Mat result;
  sut_->setOnFrameCallback([&result, &cvar](const cv::Mat& m)
    {
      result = m;
      cvar.notify_all();
    });
  ASSERT_TRUE(sut_->start());
  std::mutex m;
  std::unique_lock l(m);
  EXPECT_TRUE(cvar.wait_for(l, TEST_TIMEOUT, [&frame, &result]() { return areEqual(frame, result); }));
  sut_->stop();
}
