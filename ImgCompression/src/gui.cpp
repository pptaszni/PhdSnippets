/**
 * /Author Pawel Ptasznik
 */

#include "AsyncFrameListener.hpp"
#include "MainWindow.hpp"
#include "UDPClient.hpp"

#include <QApplication>

#include <boost/program_options.hpp>

#include <iostream>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  QApplication a(argc, argv);
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("algorithm", po::value<int>()->default_value(1),
      "type of compression algorithm");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return -1;
  }
  int alg = vm["algorithm"].as<int>();
  MainWindow w(std::make_shared<AsyncFrameListener>(std::make_shared<UDPClient>("127.0.0.1", 8181), alg));
  w.show();
  return a.exec();
}
