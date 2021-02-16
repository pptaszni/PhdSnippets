/**
 * /author Pawel Ptasznik
 */

#include "UnderwaterRobot.hpp"
#include "MsgHeader.hpp"
#include "CompressionAlgorithmTypes.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <future>
#include <memory>
#include <string>
#include <vector>

constexpr size_t HELLO_SIZE = 5;

namespace
{
cv::Mat cutFrameToRoi(const cv::Mat& m, const ROI& roi)
{
  if (roi.upperLeftX == 0 && roi.upperLeftY == 0 && roi.bottomRightX == 0 && roi.bottomRightY == 0)
  {
    return m;
  }
  int width = m.size().width;
  int height = m.size().height;
  cv::Rect rect(cv::Point(roi.upperLeftX*(width-1), roi.upperLeftY*(height-1)),
    cv::Point(roi.bottomRightX*(width-1), roi.bottomRightY*(height-1)));
  return m(rect);
}
}

UnderwaterRobot::UnderwaterRobot(std::shared_ptr<INetworkServer> publisher, UnderwaterRobotSettings settings):
  logger_{ "UnderwaterRobot" }
  , publisher_{ publisher}
  , settings_{ settings }
  , framesSource_{ settings_.framesDir, settings_.fps, settings_.imgScale }
  , stop_{ true }
  , roi_{ 0, 0, 0, 0 }
{
  logger_->info("CTor");
}

bool UnderwaterRobot::start()
{
  if (!stop_)
  {
    logger_->info("Already started");
    return true;
  }
  if (!publisher_->acceptConnection())
  {
    logger_->error("Failed to accept connection");
    return false;
  }
  uint8_t buff[HELLO_SIZE];
  publisher_->receive(buff, HELLO_SIZE);
  logger_->info("Received hello");
  stop_ = false;
  asyncReceiver_ = std::async(std::launch::async, [this]()
  {
    while (!stop_)
    {
      uint8_t buff[sizeof(ROI)];
      publisher_->receive(buff, sizeof(ROI));
      std::memcpy(&roi_, buff, sizeof(ROI));
      logger_->info("Got ROI: {}", roi_);
    }
  });
  asyncPublisher_ = std::async(std::launch::async, [this]()
  {
    while (!stop_)
    {
      publisherLoop();
    }
  });
  return true;
}

bool UnderwaterRobot::stop()
{
  if (stop_)
  {
    logger_->info("Already stopped");
    return true;
  }
  stop_ = true;
  asyncReceiver_.get();
  asyncPublisher_.get();
  publisher_->disconnect();
  return true;
}

void UnderwaterRobot::publisherLoop()
{
    cv::Mat m = cutFrameToRoi(framesSource_.getNextFrame(), roi_);
    cv::cvtColor(m, m, cv::COLOR_BGR2GRAY);
    auto buff = encodeFrame(m);
    MsgHeader h{(uint16_t)buff.size(), (uint16_t)m.size().width, (uint16_t)m.size().height, 0};
    h.crc = h.calculateCrc();
    publisher_->send((uint8_t*)&h, sizeof(h));
    publisher_->send(buff.data(), buff.size());
    cv::imshow("Preview", m);
    cv::waitKey(0);
}

std::vector<uint8_t> UnderwaterRobot::encodeFrame(cv::Mat m)
{
  std::vector<uint8_t> buff;
  if (settings_.compressionAlg == NONE)
  {
    buff.resize(m.total());
    std::memcpy(buff.data(), m.data, m.total());
    return buff;
  }
  else if (settings_.compressionAlg == JPEG)
  {
    std::vector<int> params({cv::IMWRITE_JPEG_QUALITY, 15});
    cv::imencode(".jpeg", m, buff, params);
    logger_->debug("Buff size = {}", buff.size());
    return buff;
  }
  else if (settings_.compressionAlg == PNG)
  {
    std::vector<int> params({cv::IMWRITE_PNG_COMPRESSION, 9});
    cv::imencode(".png", m, buff, params);
    logger_->debug("Buff size = {}", buff.size());
    return buff;
  }
  else
  {
    logger_->warn("Unrecognized algorithm");
    throw std::runtime_error("Unrecognized algorithm");
  }
}
