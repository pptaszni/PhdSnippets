/**
 * /author Pawel Ptasznik
 */

#include "SemanticCompression.hpp"
#include "Logger.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>

Logger logger("SemanticCompression");

void detectionTest(cv::Mat& frame)
{
  cv::Mat thresh;
  cv::adaptiveThreshold(frame, thresh, 255,
    cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
  // cv::imshow("thresh", thresh);
  // cv::waitKey(0);
  // frame = thresh;
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(thresh, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
  logger->info("contours.size() = {}", contours.size());
  logger->info("contours[0].size() = {}", contours[0].size());
  logger->info("hierarchy.size() = {}", hierarchy.size());
  std::vector<std::vector<cv::Point> > detectedSquares;
  for (const auto& contour : contours)
  {
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, cv::arcLength(contour, true)*0.05, true);
    if (approx.size() == 4 && cv::contourArea(approx) > 1000 && cv::isContourConvex(approx))
    {
      logger->info("square candidate found");
      detectedSquares.push_back(approx);
    }
  }
  cv::Mat contoursPreview = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
  for (int i = 0; i < detectedSquares.size(); i++)
  {
    cv::Scalar color(255, 0, 0);
    logger->info("Going to draw square ... ");
    for (auto sq : detectedSquares[i])
    {
      logger->info("cv point: {}", sq);
    }
    // cv::drawContours(contoursPreview, detectedSquares[i], i, color, cv::FILLED, 8);
    cv::polylines(contoursPreview, detectedSquares[i], true, (0,255,255));
    cv::polylines(frame, detectedSquares[i], true, (0,255,255));
  }
  cv::imshow("contoursPreview", contoursPreview);
  cv::waitKey(0);
}
