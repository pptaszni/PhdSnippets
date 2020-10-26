/**
 * /author Pawel Ptasznik
 */

#include "FramesFromDir.hpp"
#include "HEVCVideoEncoder.hpp"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <lz4.h>

#include <boost/program_options.hpp>

#define NUM_FRAMES 50

namespace po = boost::program_options;
using cern::algorithms::videocodecs::HEVCVideoEncoder;

void hevcTest(IVideoSource* src);
void lz4Test(IVideoSource* src);
void jpegTest(IVideoSource* src);

int main(int argc, char** argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("frames_dir", po::value<std::string>(), "directory containing consecutive frames to be used for testing")
    ("help", "produce help message")
    ("fps", po::value<float>()->default_value(1.0),
      "frames per second to be used with compression algorithms")
    ("scale", po::value<int>()->default_value(1),
      "resize the images by scale")
    ("hevc_test", "test HEVCVideoEncoder")
    ("lz4_test", "test lz4 compression algorithm")
    ("jpeg_test", "single frame cv imwrite to jpeg test");
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
  if (vm.count("hevc_test")) hevcTest(&framesSource);
  if (vm.count("lz4_test")) lz4Test(&framesSource);
  if (vm.count("jpeg_test")) jpegTest(&framesSource);
  return 0;
}

void hevcTest(IVideoSource* src)
{
  cv::Mat m = src->getNextFrame();
  cv::imshow("frame", m);
  cv::waitKey(0);
  HEVCVideoEncoder enc(m.size(), src->getFps(), HEVCVideoEncoder::Normal, false);
  for (int i = 0; i < NUM_FRAMES; i++)
  {
    m = src->getNextFrame();
    enc.addFrame(m);
    std::this_thread::sleep_for(std::chrono::duration<float>(1/src->getFps()));
  }
  std::string buff = enc.getBytes();
  std::cout << buff.size() << std::endl;
}

void lz4Test(IVideoSource* src)
{
  cv::Mat m = src->getNextFrame();
  size_t singleframeSize = m.total() * m.elemSize();
  std::unique_ptr<char[]> origBuff(new char[singleframeSize]);
  std::memcpy(origBuff.get(), m.data, singleframeSize);
  std::unique_ptr<char[]> compressedBuff(new char[LZ4_compressBound(singleframeSize)]);
  int compressedSize = LZ4_compress_default(origBuff.get(), compressedBuff.get(),
    singleframeSize, LZ4_compressBound(singleframeSize));
  std::cout << "single frame compression: " << compressedSize << std::endl;
  origBuff.reset(new char[singleframeSize*NUM_FRAMES]);
  for (int i = 0; i < NUM_FRAMES; i++)
  {
    m = src->getNextFrame();
    std::memcpy(origBuff.get()+i*singleframeSize, m.data, singleframeSize);
    std::this_thread::sleep_for(std::chrono::duration<float>(1/src->getFps()));
  }
  compressedBuff.reset(new char[LZ4_compressBound(singleframeSize*NUM_FRAMES)]);
  compressedSize = LZ4_compress_default(origBuff.get(), compressedBuff.get(),
    singleframeSize*NUM_FRAMES, LZ4_compressBound(singleframeSize*NUM_FRAMES));
  std::cout << "multiple frame compression: " << compressedSize << std::endl;
}

void jpegTest(IVideoSource* src)
{
  // just one frame test, because N frames it is just N times more
  cv::Mat m = src->getNextFrame();
  size_t singleframeSize = m.total() * m.elemSize();
  std::vector<uchar> buff;
  std::vector<int> params;
  params.push_back(cv::IMWRITE_JPEG_QUALITY);
  params.push_back(10);
  cv::imencode(".jpeg", m, buff, params);
  std::cout << "jpeg size: " << buff.size() << std::endl;
}
