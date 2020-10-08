/* © Copyright CERN 2019.  All rights reserved. This software is released under a CERN proprietary software licence.
 * Any permission to use it shall be granted in writing. Requests shall be addressed to CERN through mail-KT@cern.ch
 *
 * Author: Giacomo Lunghi CERN EN/SMM/MRO
 *
 *  ==================================================================================================
*/

#include <string>

#include "VideoCodecs/x264VideoEncoder.hpp"

namespace cern {
namespace algorithms {
namespace videocodecs {

x264VideoEncoder::x264VideoEncoder(cv::Size resolution,
        CompressionQuality quality, bool zero_latency) :
    logger_("x264VideoEncoder"),
    resolution_(resolution),
    zerolatency_(zero_latency) {
        logger_->debug("CTor");

        std::string tune = zero_latency ? "zerolatency" : "film";

        if (x264_param_default_preset(&parameters_, x264_preset_names[quality], tune.c_str()) < 0) {
            throw std::runtime_error("Could not set x264 parameters");
        }

        parameters_.i_csp = X264_CSP_I420;
        parameters_.i_width  = resolution_.width;
        parameters_.i_height = resolution_.height;
        parameters_.b_vfr_input = 0;
        parameters_.b_repeat_headers = 1;
        parameters_.b_annexb = 1;
        parameters_.i_log_level = X264_LOG_ERROR;

        if (x264_param_apply_profile(&parameters_, "main") < 0) {
            throw std::runtime_error("Could not set x264 parameters");
        }

        if (x264_picture_alloc(
            &picture_, parameters_.i_csp, parameters_.i_width, parameters_.i_height) < 0) {
                throw std::runtime_error("Could not set x264 parameters");
        }

        if (x264_picture_alloc(
            &pictureOut_, parameters_.i_csp, parameters_.i_width, parameters_.i_height) < 0) {
                throw std::runtime_error("Could not set x264 parameters");
        }

        encoder_ = x264_encoder_open(&parameters_);

        if (!encoder_) {
            throw std::runtime_error("Could not open x264 encoder");
        }
}

x264VideoEncoder::~x264VideoEncoder() {
    logger_->debug("DTor");

    /** Necessary cleanup before calling x264_picture_clean
     * otherwise it goes to try to destroy the latest cv::Mat
     **/ 
    picture_.img.i_plane = 0;
    picture_.img.plane[0] = 0;
    picture_.img.plane[1] = 0;
    picture_.img.plane[2] = 0;

    x264_picture_clean(&picture_);

    pictureOut_.img.i_plane = 0;
    pictureOut_.img.plane[0] = 0;
    pictureOut_.img.plane[1] = 0;
    pictureOut_.img.plane[2] = 0;

    x264_picture_clean(&pictureOut_);

    x264_encoder_close(encoder_);
}

bool x264VideoEncoder::addFrame(const cv::Mat& frame) {
    logger_->debug("addFrame()");
    if ((frame.size().width != resolution_.width) ||
        (frame.size().height != resolution_.height)) {
            logger_->warn("Frame and encoders have different resolution");
            return false;
    }

    cv::Mat convertedFrame;
    cv::cvtColor(frame, convertedFrame, cv::COLOR_BGR2YUV_I420);

    int luma_size = resolution_.width*resolution_.height;
    int chroma_size = luma_size / 4;
    picture_.img.i_plane = 3;
    picture_.img.plane[0] = convertedFrame.data;
    picture_.img.plane[1] = picture_.img.plane[0] + luma_size;
    picture_.img.plane[2] = picture_.img.plane[1] + chroma_size;

    x264_nal_t *nal;
    int i_nal;

    std::lock_guard<std::mutex> lock(flowBytesMutex);
    if (x264_encoder_encode(encoder_, &nal, &i_nal, &picture_, &pictureOut_) < 0) {
        logger_->error("Could not encode picture");
        return false;
    }

    if (i_nal > 0) {
        for (int i=0; i < i_nal; i++) {
            std::string pp_nal_string(reinterpret_cast<char*>(nal[i].p_payload),
                nal[i].i_payload);

            flowBytes.append(pp_nal_string);
        }
    }

    return true;
}

bool x264VideoEncoder::flush() {
    logger_->debug("flush()");

    if (zerolatency_) {
        logger_->warn("Can't flush in zerolatency mode");
        return false;
    }

    std::lock_guard<std::mutex> lock(flowBytesMutex);
    while (x264_encoder_delayed_frames(encoder_)) {     // Flush delayed frames
        x264_nal_t *nal;
        int i_nal;
        if (x264_encoder_encode(encoder_, &nal, &i_nal, NULL, &pictureOut_) < 0) {
            logger_->error("Could not encode picture");
            return false;
        }
        if (i_nal > 0) {
            for (int i=0; i < i_nal; i++) {
                std::string pp_nal_string(reinterpret_cast<char*>(nal[i].p_payload),
                    nal[i].i_payload);
                flowBytes.append(pp_nal_string);
            }
        }
    }

    return true;
}

std::string x264VideoEncoder::getBytes(bool clearBuffer) {
    logger_->debug("getBytes()");

    std::lock_guard<std::mutex> lock(flowBytesMutex);
    std::string returnString(flowBytes);

    if (clearBuffer)
        flowBytes.clear();

    return returnString;
}

}  // namespace videocodecs
}  // namespace algorithms
}  // namespace cern
