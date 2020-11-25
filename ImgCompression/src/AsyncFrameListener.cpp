/**
 * /author Pawel Ptasznik
 */

#include "AsyncFrameListener.hpp"

#include <opencv2/core.hpp>

#include <chrono>
#include <cstdint>
#include <future>
#include <thread>

constexpr auto SLEEP_TIME = std::chrono::milliseconds(1);

bool operator==(const ROI& left, const ROI& right)
{
  return (left.upperLeftX == right.upperLeftX) &&
    (left.upperLeftY == right.upperLeftY) &&
    (left.bottomRightX == right.bottomRightX) &&
    (left.bottomRightY == right.bottomRightY);
}

bool operator!=(const ROI& left, const ROI& right)
{
  return !(left == right);
}
std::ostream& operator<<(std::ostream& stream, const ROI& roi)
{
  stream << "[ " << roi.upperLeftX << ", " << roi.upperLeftY
    << ", " << roi.bottomRightX << ", " << roi.bottomRightY << " ]";
    return stream;
}

AsyncFrameListener::AsyncFrameListener(std::shared_ptr<INetworkClient> networkClient): logger_("AsyncFrameListener")
  , networkClient_(networkClient)
  , stop_(true)
  , cb_(nullptr)
  , asyncTask_()
{
  logger_->debug("CTor");
}

AsyncFrameListener::~AsyncFrameListener()
{
  stop();
}

void AsyncFrameListener::setOnFrameCallback(Callback cb)
{
  logger_->debug("setOnFrameCallback");
  cb_ = cb;
}

bool AsyncFrameListener::start()
{
  logger_->debug("start");
  if (asyncTask_.valid())
  {
    logger_->warn("Task already running");
    return false;
  }
  if (!networkClient_->connect())
  {
    logger_->warn("Failed to connect network client");
    return false;
  }
  stop_ = false;
  asyncTask_ = std::async(std::launch::async, [this]()
    {
      networkClient_->send((const uint8_t*)"Hello", 5);
      while (!stop_)
      {
        asyncLoop();
      }
    });
  return true;
}

bool AsyncFrameListener::stop()
{
  logger_->debug("stop");
  if (!asyncTask_.valid())
  {
    logger_->warn("Task already stopped");
    return false;
  }
  stop_ = true;
  asyncTask_.get();
  networkClient_->disconnect();
  return true;
}

void AsyncFrameListener::requestRoi(const ROI& roi)
{
  logger_->debug("Requested new ROI: {}", roi);
  uint8_t buff[sizeof(ROI)];
  std::memcpy(buff, &roi, sizeof(ROI));
  networkClient_->send(buff, sizeof(ROI));
}

void AsyncFrameListener::asyncLoop()
{
  std::this_thread::sleep_for(SLEEP_TIME);
  uint8_t header_buff[sizeof(MsgHeader)];
  int result = networkClient_->receive(header_buff, sizeof(MsgHeader));
  // TODO: no error handling, we just fall through is sth goes wrong, maybe later correct it
  if (result != sizeof(MsgHeader))
  {
    logger_->warn("received {} instead of {}", result, sizeof(MsgHeader));
  }
  MsgHeader header;
  std::memcpy(&header, header_buff, sizeof(MsgHeader));
  if (header.crc != header.calculateCrc())
  {
    logger_->warn("CRC mismatch, expected {} vs {}", header.calculateCrc(), header.crc);
  }
  logger_->debug("Going to receive {} data for {}x{} frame",
    header.payload, header.width, header.height);
  std::unique_ptr<uint8_t[]> buff(new uint8_t[header.payload]);
  result = networkClient_->receive(buff.get(), header.payload);
  if (result != header.payload)
  {
    logger_->warn("received {} instead of {}", result, header.payload);
  }
  if (!cb_) return;
  cv::Mat frame(header.width, header.height, CV_8UC1, cv::Scalar(0));
  std::memcpy(frame.data, buff.get(), result);
  cb_(frame);
}
