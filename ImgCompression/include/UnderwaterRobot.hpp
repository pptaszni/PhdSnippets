/**
 * /author Pawel Ptasznik
 */

#include "FramesFromDir.hpp"
#include "INetworkServer.hpp"
#include "Logger.hpp"
#include "ROI.hpp"

#include <opencv2/core.hpp>

#include <atomic>
#include <future>
#include <memory>
#include <string>

struct UnderwaterRobotSettings
{
  float fps;
  int imgScale;
  std::string framesDir;
  int compressionAlg;
};

class UnderwaterRobot
{
public:
  UnderwaterRobot(std::shared_ptr<INetworkServer> publisher, UnderwaterRobotSettings settings);
  bool start();
  bool stop();

private:
  void publisherLoop();
  std::vector<uint8_t> encodeFrame(cv::Mat m);
  Logger logger_;
  std::shared_ptr<INetworkServer> publisher_;
  UnderwaterRobotSettings settings_;
  FramesFromDir framesSource_;
  std::atomic_bool stop_;
  ROI roi_;
  std::future<void> asyncReceiver_;
  std::future<void> asyncPublisher_;
};
