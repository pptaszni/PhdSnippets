#pragma once

/* © Copyright CERN 2019.  All rights reserved. This software is released under a CERN proprietary software licence.
 * Any permission to use it shall be granted in writing. Requests shall be addressed to CERN through mail-KT@cern.ch
 *
 * Author: Giacomo Lunghi CERN EN/SMM/MRO
 *
 *  ==================================================================================================
*/

#include <string>
#include <opencv2/opencv.hpp>

namespace cern {
namespace algorithms {
namespace videocodecs {

class IVideoEncoder {
 public:
    virtual ~IVideoEncoder() = default;

    virtual bool addFrame(const cv::Mat&) = 0;
    virtual std::string getBytes(bool clearBuffer = true) = 0;
    virtual bool flush() = 0;
};

}  // namespace videocodecs
}  // namespace algorithms
}  // namespace cern
