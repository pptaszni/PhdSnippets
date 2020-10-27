/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "INetworkClient.hpp"
#include "Logger.hpp"

#include <opencv2/core.hpp>

#include <future>
#include <memory>

class AsyncFrameListener
{
public:
  using Callback = std::function<void(cv::Mat frame)>;
  AsyncFrameListener(std::shared_ptr<INetworkClient> networkClient);
  ~AsyncFrameListener();
  void setOnFrameCallback(Callback cb);
  bool start();
  bool stop();

private:
  Logger logger_;
  std::shared_ptr<INetworkClient> networkClient_;
  Callback cb_;
  std::future<void> asyncTask_;
};
