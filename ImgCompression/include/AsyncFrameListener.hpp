/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "INetworkClient.hpp"
#include "Logger.hpp"
#include "MsgHeader.hpp"
#include "ROI.hpp"

#include <opencv2/core.hpp>

#include <atomic>
#include <cstdint>
#include <future>
#include <iostream>
#include <memory>
#include <vector>

class AsyncFrameListener
{
public:
  using Callback = std::function<void(const cv::Mat& frame)>;
  AsyncFrameListener(std::shared_ptr<INetworkClient> networkClient, int alg);
  // virtual for mocking, too lazy to do another interface
  virtual ~AsyncFrameListener();
  virtual void setOnFrameCallback(Callback cb);
  virtual bool start();
  virtual bool stop();
  virtual void requestRoi(const ROI& roi);

private:
  void asyncLoop();
  cv::Mat decodeFrame(const MsgHeader& h, const std::vector<uint8_t>& data);
  Logger logger_;
  std::shared_ptr<INetworkClient> networkClient_;
  int algorithm_;
  std::atomic_bool stop_;
  Callback cb_;
  std::future<void> asyncTask_;
};
