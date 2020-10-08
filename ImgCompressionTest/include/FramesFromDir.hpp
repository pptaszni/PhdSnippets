/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "IVideoSource.hpp"

#include <opencv2/core.hpp>

class FramesFromDir: public IVideoSource
{
public:
  explicit FramesFromDir(std::string dirName, float fps = 1, int scale = 1);
  ~FramesFromDir() override;
  cv::Mat getNextFrame() override;
  float getFps() override;

private:
  std::string _dirName;
  std::vector<cv::Mat> _frames;
  int _currentFrameIdx;
  float _fps;
  std::chrono::time_point<std::chrono::steady_clock> _startPoint;
};
