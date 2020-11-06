/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "INetworkClient.hpp"
#include "Logger.hpp"

#include <opencv2/core.hpp>

#include <atomic>
#include <cstdint>
#include <future>
#include <iostream>
#include <memory>

struct MsgHeader
{
  uint16_t payload;
  uint16_t width;
  uint16_t height;
  uint8_t crc;
  uint8_t calculateCrc() { return (payload + width + height) % 255; }
};

struct ROI
{
  int upperLeftX;
  int upperLeftY;
  int bottomRightX;
  int bottomRightY;
};
bool operator==(const ROI& left, const ROI& right);
bool operator!=(const ROI& left, const ROI& right);
std::ostream& operator<<(std::ostream& stream, const ROI& roi);

class AsyncFrameListener
{
public:
  using Callback = std::function<void(const cv::Mat& frame)>;
  AsyncFrameListener(std::shared_ptr<INetworkClient> networkClient);
  // virtual for mocking, too lazy to do another interface
  virtual ~AsyncFrameListener();
  virtual void setOnFrameCallback(Callback cb);
  virtual bool start();
  virtual bool stop();
  virtual void requestRoi(const ROI& roi);

private:
  void asyncLoop();
  Logger logger_;
  std::shared_ptr<INetworkClient> networkClient_;
  std::atomic_bool stop_;
  Callback cb_;
  std::future<void> asyncTask_;
};
