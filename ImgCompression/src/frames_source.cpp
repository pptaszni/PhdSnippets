/**
 * /author Pawel Ptasznik
 */

#include "FramesFromDir.hpp"
#include "UDPServer.hpp"
#include "MsgHeader.hpp"
#include "ROI.hpp"

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <thread>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

// TODO: make some publisher class, it should handle ROI requests and publish frames continuously
cv::Mat cutFrameToRoi(const cv::Mat& m, const ROI& roi)
{
  if (roi.upperLeftX == 0 && roi.upperLeftY == 0 && roi.bottomRightX == 0 && roi.bottomRightY == 0)
  {
    return m;
  }
  int width = m.size().width;
  int height = m.size().height;
  cv::Rect rect(cv::Point(roi.upperLeftX*(width-1), roi.upperLeftY*(height-1)),
    cv::Point(roi.bottomRightX*(width-1), roi.bottomRightY*(height-1)));
  return m(rect);
}

int main(int argc, char** argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("frames_dir", po::value<std::string>(), "directory containing consecutive frames to be used for testing")
    ("help", "produce help message")
    ("fps", po::value<float>()->default_value(1.0),
      "frames per second to be used with compression algorithms")
    ("scale", po::value<int>()->default_value(20),
      "resize the images by scale");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return -1;
  }
  if (!vm.count("frames_dir")) {
    std::cout << "frames_dir option is mandatory" << std::endl;
    std::cout << desc << std::endl;
    return -1;
  }
  float fps = vm["fps"].as<float>();
  int scale = vm["scale"].as<int>();
  FramesFromDir framesSource(vm["frames_dir"].as<std::string>(), fps, scale);
  UDPServer server(8181);
  if (!server.acceptConnection())
  {
    std::cout << "Failed to start UDP server" << std::endl;
    return -1;
  }
  uint8_t buff[5];
  server.receive(buff, 5);
  char c(0);
  ROI roi{0, 0, 0, 0};
  // TODO move this logic to the class and do some sync
  auto future = std::async(std::launch::async, [&server, &c, &roi]()
  {
    while (c != 'q')
    {
      uint8_t buff[sizeof(ROI)];
      server.receive(buff, sizeof(ROI));
      std::memcpy(&roi, buff, sizeof(ROI));
      std::cout << "Got ROI: " << roi << std::endl;
    }
  });
  while (c != 'q')
  {
    cv::Mat m = cutFrameToRoi(framesSource.getNextFrame(), roi);
    cv::cvtColor(m, m, cv::COLOR_BGR2GRAY);
    MsgHeader h{(uint16_t)m.total(), (uint16_t)m.size().width, (uint16_t)m.size().height, 0};
    h.crc = h.calculateCrc();
    server.send((uint8_t*)&h, sizeof(h));
    server.send(m.data, m.total());
    cv::imshow("Preview", m);
    cv::waitKey(0);
  }
  return 0;
}
