/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "INetworkServer.hpp"
#include "Logger.hpp"

#include <cstdint>
#include <string>

#include <sys/types.h>
#include <netinet/ip.h>

class UDPServer: public INetworkServer {
 public:
  UDPServer(int port);
  ~UDPServer() override = default;
  bool acceptConnection(bool blocking = true) override;
  bool disconnect() override;
  bool setPort(uint16_t port) override;
  int send(const uint8_t* data, size_t length) override;
  int receive(uint8_t* data, size_t length) override;
  bool isConnected() override;

private:
  Logger logger_;
  int port_;
  int sockfd_;
  struct sockaddr_in servaddr_;
  struct sockaddr_in cliaddr_;
};
