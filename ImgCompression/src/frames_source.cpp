/**
 * /author Pawel Ptasznik
 */

#include "UDPServer.hpp"
#include "UnderwaterRobot.hpp"

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
constexpr int PORT = 8181;


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
  UnderwaterRobot robot(std::make_shared<UDPServer>(PORT), {fps, scale, vm["frames_dir"].as<std::string>()});
  if (!robot.start())
  {
    return -1;
  }
  char c(0);
  while (c != 'q')
  {
    std::cin >> c;
  }
  robot.stop();
  return 0;
}
