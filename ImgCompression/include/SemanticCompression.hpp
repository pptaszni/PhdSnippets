/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <vector>

#include <opencv2/core.hpp>

void detectionTest(cv::Mat& frame);
std::vector<cv::Point2f> surfDetectionTest(const cv::Mat& frame);
cv::Mat surfDecode(const std::vector<uint8_t>& data);
