/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <opencv2/core.hpp>

class IVideoSource
{
public:
  virtual ~IVideoSource() = default;
  virtual cv::Mat getNextFrame() = 0;
  virtual float getFps() = 0;
};
