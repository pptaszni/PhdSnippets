/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "INetworkClient.hpp"
#include "Logger.hpp"

#include <opencv2/core.hpp>

#include <atomic>
#include <future>
#include <memory>
#include <iostream>

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
  ~AsyncFrameListener();
  void setOnFrameCallback(Callback cb);
  bool start();
  bool stop();
  void requestRoi(const ROI& roi);

private:
  void asyncLoop();
  Logger logger_;
  std::shared_ptr<INetworkClient> networkClient_;
  std::atomic_bool stop_;
  cv::Mat frame_;
  Callback cb_;
  std::future<void> asyncTask_;
};
