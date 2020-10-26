#pragma once

/* © Copyright CERN 2019.  All rights reserved. This software is released under a CERN proprietary software licence.
 * Any permission to use it shall be granted in writing. Requests shall be addressed to CERN through mail-KT@cern.ch
 *
 * Author: Giacomo Lunghi CERN EN/SMM/MRO
 *
 *  ==================================================================================================
*/

#include "IVideoEncoder.hpp"

#include <x265.h>
#include <x265_config.h>

#include <opencv2/opencv.hpp>

#include <mutex>
#include <string>

namespace cern {
namespace algorithms {
namespace videocodecs {

class HEVCVideoEncoder : IVideoEncoder {
 public:
    enum CompressionQuality {
        Ultrafast = 0, Superfast = 1, Veryfast = 2, Faster = 3,
        Fast = 4, Normal = 5, Slow = 6, Slower = 7, VerySlow = 8 };
    HEVCVideoEncoder() = delete;
    HEVCVideoEncoder(cv::Size resolution, float framerate,
        CompressionQuality quality, bool zerolatency);
    ~HEVCVideoEncoder() override;

    bool addFrame(const cv::Mat&) override;
    bool flush() override;
    std::string getBytes(bool clearBuffer = true) override;

 private:

    cv::Size resolution_;
    float framerate_;

    bool flushed_;
    bool zerolatency_;

    x265_encoder* encoder_;
    x265_param* parameters_;
    x265_picture* picture_;

    std::mutex flowBytesMutex;
    std::string flowBytes;
};

}  // namespace videocodecs
}  // namespace algorithms
}  // namespace cern
