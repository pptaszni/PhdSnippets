/**
 * /author Pawel Ptasznik
 */

#include "SemanticCompression.hpp"

#include <algorithm>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <vector>

#include "Logger.hpp"

constexpr auto OBJ_MODEL_FILENAME = "red_box.png";
constexpr int MIN_HESSIAN = 300;
// same dimentions as in GUI to match the corners coordinates
constexpr int FRAME_WIDTH = 960;
constexpr int FRAME_HEIGHT = 540;

Logger logger("SemanticCompression");

void detectionTest(cv::Mat& frame) {
  cv::Mat thresh;
  cv::adaptiveThreshold(frame, thresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                        cv::THRESH_BINARY, 11, 2);
  // cv::imshow("thresh", thresh);
  // cv::waitKey(0);
  // frame = thresh;
  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(thresh, contours, hierarchy, cv::RETR_TREE,
                   cv::CHAIN_APPROX_NONE);
  logger->info("contours.size() = {}", contours.size());
  logger->info("contours[0].size() = {}", contours[0].size());
  logger->info("hierarchy.size() = {}", hierarchy.size());
  std::vector<std::vector<cv::Point> > detectedSquares;
  for (const auto& contour : contours) {
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, cv::arcLength(contour, true) * 0.05,
                     true);
    if (approx.size() == 4 && cv::contourArea(approx) > 1000 &&
        cv::isContourConvex(approx)) {
      logger->info("square candidate found");
      detectedSquares.push_back(approx);
    }
  }
  cv::Mat contoursPreview = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC3);
  for (int i = 0; i < detectedSquares.size(); i++) {
    cv::Scalar color(255, 0, 0);
    logger->info("Going to draw square ... ");
    for (auto sq : detectedSquares[i]) {
      logger->info("cv point: {}", sq);
    }
    // cv::drawContours(contoursPreview, detectedSquares[i], i, color,
    // cv::FILLED, 8);
    cv::polylines(contoursPreview, detectedSquares[i], true, (0, 255, 255));
    cv::polylines(frame, detectedSquares[i], true, (0, 255, 255));
  }
  cv::imshow("contoursPreview", contoursPreview);
  cv::waitKey(0);
}

std::vector<cv::Point2f> surfDetectionTest(const cv::Mat& frame) {
  logger->info("Going to run surfDetectionTest with model image: {}",
               OBJ_MODEL_FILENAME);
  cv::Mat objModel = cv::imread(cv::samples::findFile(OBJ_MODEL_FILENAME),
                                cv::IMREAD_GRAYSCALE);
  cv::Mat objInContext;
  cv::resize(frame, objInContext, cv::Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0,
             cv::INTER_CUBIC);
  auto detector = cv::xfeatures2d::SURF::create(MIN_HESSIAN);
  // cv::imshow("objModel", objModel);
  // cv::imshow("objInContext", objInContext);
  // cv::waitKey();
  std::vector<cv::KeyPoint> keypointsModel, keypointsFrame;
  cv::Mat descriptorsModel, descriptorsFrame;
  detector->detectAndCompute(objModel, cv::noArray(), keypointsModel,
                             descriptorsModel);
  detector->detectAndCompute(objInContext, cv::noArray(), keypointsFrame,
                             descriptorsFrame);
  auto matcher =
      cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
  std::vector<std::vector<cv::DMatch> > knnMatches;
  logger->info("Going to run KNN matcher");
  matcher->knnMatch(descriptorsModel, descriptorsFrame, knnMatches, 2);
  //-- Filter matches using the Lowe's ratio test
  const float ratio_thresh = 0.7f;
  std::vector<cv::DMatch> goodMatches;
  for (size_t i = 0; i < knnMatches.size(); i++) {
    if (knnMatches[i][0].distance < ratio_thresh * knnMatches[i][1].distance) {
      goodMatches.push_back(knnMatches[i][0]);
    }
  }
  logger->info("goodMatches size: {}", goodMatches.size());
  // just for debugging purposes
  cv::Mat debugImgMatches;
  cv::drawMatches(objModel, keypointsModel, objInContext, keypointsFrame,
                  goodMatches, debugImgMatches, cv::Scalar::all(-1),
                  cv::Scalar::all(-1), std::vector<char>(),
                  cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
  // cv::imshow("Good Matches", debugImgMatches);
  // cv::waitKey();
  std::vector<cv::Point2f> obj;
  std::vector<cv::Point2f> scene;
  for (size_t i = 0; i < goodMatches.size(); i++) {
    //-- Get the keypoints from the good matches
    obj.push_back(keypointsModel[goodMatches[i].queryIdx].pt);
    scene.push_back(keypointsFrame[goodMatches[i].trainIdx].pt);
  }
  cv::Mat homography = cv::findHomography(obj, scene, cv::RANSAC);
  //-- Get the corners from the image_1 ( the object to be "detected" )
  std::vector<cv::Point2f> objCorners(4);
  objCorners[0] = cv::Point2f(0, 0);
  objCorners[1] = cv::Point2f((float)objModel.cols, 0);
  objCorners[2] = cv::Point2f((float)objModel.cols, (float)objModel.rows);
  objCorners[3] = cv::Point2f(0, (float)objModel.rows);
  std::vector<cv::Point2f> sceneCorners(4);
  cv::perspectiveTransform(objCorners, sceneCorners, homography);
  // objModel.cols offset added, because 2 pictures are shown next to eachother
  cv::line(debugImgMatches,
           sceneCorners[0] + cv::Point2f((float)objModel.cols, 0),
           sceneCorners[1] + cv::Point2f((float)objModel.cols, 0),
           cv::Scalar(0, 255, 0), 4);
  cv::line(debugImgMatches,
           sceneCorners[1] + cv::Point2f((float)objModel.cols, 0),
           sceneCorners[2] + cv::Point2f((float)objModel.cols, 0),
           cv::Scalar(0, 255, 0), 4);
  cv::line(debugImgMatches,
           sceneCorners[2] + cv::Point2f((float)objModel.cols, 0),
           sceneCorners[3] + cv::Point2f((float)objModel.cols, 0),
           cv::Scalar(0, 255, 0), 4);
  cv::line(debugImgMatches,
           sceneCorners[3] + cv::Point2f((float)objModel.cols, 0),
           sceneCorners[0] + cv::Point2f((float)objModel.cols, 0),
           cv::Scalar(0, 255, 0), 4);
  //-- Show detected matches
  // cv::imshow("Good Matches & Object detection", debugImgMatches);
  // cv::waitKey();
  return sceneCorners;
}

cv::Mat surfDecode(const std::vector<uint8_t>& data) {
  std::vector<cv::Point2f> corners(4);
  size_t cornersSize = sizeof(cv::Point2f) * corners.size();
  std::memcpy(corners.data(), data.data() + (data.size() - cornersSize),
              cornersSize);
  std::vector<uint8_t> rawFrame(data.begin(), data.end() - cornersSize);
  cv::Mat finalFrame = cv::imdecode(data, cv::IMREAD_UNCHANGED);
  cv::resize(finalFrame, finalFrame, cv::Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0,
             cv::INTER_CUBIC);
  cv::line(finalFrame, corners[0], corners[1], cv::Scalar(0, 255, 0), 4);
  cv::line(finalFrame, corners[1], corners[2], cv::Scalar(0, 255, 0), 4);
  cv::line(finalFrame, corners[2], corners[3], cv::Scalar(0, 255, 0), 4);
  cv::line(finalFrame, corners[3], corners[0], cv::Scalar(0, 255, 0), 4);
  return finalFrame;
}
