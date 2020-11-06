/**
 * /Author Pawel Ptasznik
 */

#include "AsyncFrameListener.hpp"
#include "MainWindow.hpp"
#include "UDPClient.hpp"

#include <QApplication>


int main(int argc, char** argv)
{
  QApplication a(argc, argv);
  MainWindow w(std::make_shared<AsyncFrameListener>(std::make_shared<UDPClient>("127.0.0.1", 8181)));
  w.show();
  return a.exec();
}
