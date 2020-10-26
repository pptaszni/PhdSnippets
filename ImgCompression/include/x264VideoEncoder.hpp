/**
 * /author Pawel Ptasznik
 */

#pragma once

/* © Copyright CERN 2019.  All rights reserved. This software is released under a CERN proprietary software licence.
 * Any permission to use it shall be granted in writing. Requests shall be addressed to CERN through mail-KT@cern.ch
 *
 * Author: Giacomo Lunghi CERN EN/SMM/MRO
 *
 *  ==================================================================================================
*/

#include <string>

#include <x264.h>
#include "Logger/Logger.hpp"
#include "VideoCodecs/IVideoEncoder.hpp"

namespace cern {
namespace algorithms {
namespace videocodecs {

class x264VideoEncoder : IVideoEncoder {
 public:
    enum CompressionQuality {
        Ultrafast = 0, Superfast = 1, Veryfast = 2, Faster = 3,
        Fast = 4, Normal = 5, Slow = 6, Slower = 7, VerySlow = 8 };

    x264VideoEncoder() = delete;
    x264VideoEncoder(cv::Size resolution,
        CompressionQuality quality, bool zero_latency);
    ~x264VideoEncoder() override;

    bool addFrame(const cv::Mat&) override;
    bool flush() override;
    std::string getBytes(bool clearBuffer = true) override;

 private:
    utility::Logger logger_;

    cv::Size resolution_;

    bool zerolatency_;

    x264_t* encoder_;
    x264_param_t parameters_;
    x264_picture_t picture_;
    x264_picture_t pictureOut_;

    std::mutex flowBytesMutex;
    std::string flowBytes;
};

}  // namespace videocodecs
}  // namespace algorithms
}  // namespace cern
