/**
 * /author Pawel Ptasznik
 */

#include "UDPClient.hpp"

#include <cstring>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

UDPClient::UDPClient(const std::string& addr, int port): logger_("UDPClient")
  , addr_(addr)
  , port_(port)
  , sockfd_(0)
  , servaddr_{}
{
  logger_->debug("CTor({})", addr);
  std::memset(&servaddr_, 0, sizeof(servaddr_));
  servaddr_.sin_family = AF_INET;
  servaddr_.sin_port = htons(port_);
  servaddr_.sin_addr.s_addr = INADDR_ANY;
}

bool UDPClient::connect(bool blocking)
{
  sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd_ < 0 )
  {
    logger_->error("Socket creation failed");
    sockfd_ = 0;
    return false;
  }
  logger_->debug("Connection SUCCESS");
  return true;
}

bool UDPClient::disconnect()
{
  close(sockfd_);
  return true;
}

bool UDPClient::setDestination(const std::string& address, uint16_t port)
{
  return false;
}

int UDPClient::send(const uint8_t* data, size_t length)
{
  logger_->debug("Sending data: {}", length);
  return sendto(sockfd_, (const char *)data, length, MSG_CONFIRM,
    (const struct sockaddr *) &servaddr_, sizeof(servaddr_));
  return 0;
}

int UDPClient::receive(uint8_t* data, size_t length)
{
  logger_->debug("Receiving data: {}", length);
  struct sockaddr_in src_addr;
  socklen_t len = sizeof(src_addr);
  int result = recvfrom(sockfd_, (char *)data, length, MSG_WAITALL, (struct sockaddr*) &src_addr, &len);
  logger_->debug("Got {} data from {}", result, inet_ntoa(src_addr.sin_addr));
  return result;
}

bool UDPClient::isConnected() const
{
  return false;
}
