/**
 * /author Pawel Ptasznik
 */

#include "UDPServer.hpp"

#include <cstring>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

UDPServer::UDPServer(int port): logger_("UDPServer")
  , port_(port)
  , sockfd_(0)
  , servaddr_{}
  , cliaddr_{}
{
  logger_->debug("CTor");
  std::memset(&servaddr_, 0, sizeof(servaddr_));
  std::memset(&cliaddr_, 0, sizeof(cliaddr_));
  servaddr_.sin_family = AF_INET;
  servaddr_.sin_addr.s_addr = INADDR_ANY;
  servaddr_.sin_port = htons(port_);
  cliaddr_.sin_family = AF_INET;
  cliaddr_.sin_addr.s_addr = INADDR_ANY;
  cliaddr_.sin_port = htons(port_);
}

bool UDPServer::acceptConnection(bool blocking)
{
  sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ < 0 )
  {
    logger_->error("Socket creation failed");
    sockfd_ = 0;
    return false;
  }
  if (bind(sockfd_, (const struct sockaddr *)&servaddr_, sizeof(servaddr_)) < 0 )
  {
    logger_->error("Bind failed because: {}", strerror(errno));
    return false;
  }
  return true;
}

bool UDPServer::disconnect()
{
  return false;
}

bool UDPServer::setPort(uint16_t port)
{
  return false;
}

int UDPServer::send(const uint8_t* data, size_t length)
{
  logger_->debug("Sending data: {}", length);
  return sendto(sockfd_, (const char *)data, length, 0,
    (const struct sockaddr *) &cliaddr_, sizeof(cliaddr_));
  return 0;
}

int UDPServer::receive(uint8_t* data, size_t length)
{
  logger_->debug("Receiving data: {}", length);
  socklen_t len = sizeof(cliaddr_);
  int result = recvfrom(sockfd_, (char *)data, length, MSG_WAITALL, (struct sockaddr*) &cliaddr_, &len);
  logger_->debug("Got {} data from {}", result, inet_ntoa(cliaddr_.sin_addr));
  return result;
}

bool UDPServer::isConnected()
{
  return false;
}
