/**
 * /author Pawel Ptasznik
 */

#include "FramesFromDir.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


FramesFromDir::FramesFromDir(std::string dirName, float fps, int scale):
  _dirName(std::move(dirName)),
  _frames(),
  _currentFrameIdx(0),
  _fps(fps),
  _startPoint(std::chrono::steady_clock::now())
{
  if (!std::filesystem::exists(_dirName))
  {
    throw std::runtime_error("Directory: " + _dirName + " does not exist");
  }
  std::cout << "fps = " << fps << ", scale = " << scale << std::endl;
  for (const auto & entry : std::filesystem::directory_iterator(_dirName))
  {
    cv::Mat m = cv::imread(entry.path().string());
    if (scale != 1)
    {
      cv::resize(m, m, m.size()/scale);
    }
    if (m.data == nullptr)
    {
      throw std::runtime_error("Failed to read image from: " + entry.path().string());
    }
    _frames.push_back(m);
    std::cout << "Added frame :" << entry.path() << std::endl;
  }
}

FramesFromDir::~FramesFromDir() {}

cv::Mat FramesFromDir::getNextFrame()
{
  if (_frames.empty())
  {
    throw std::runtime_error("No frames!");
  }
  std::chrono::duration<float> diff = std::chrono::steady_clock::now() - _startPoint;
  int frameIdx = static_cast<int>(diff.count()*_fps) % _frames.size();
  return _frames[frameIdx];
}

float FramesFromDir::getFps()
{
  return _fps;
}
