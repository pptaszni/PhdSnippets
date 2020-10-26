/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "INetworkClient.hpp"
#include "Logger.hpp"

#include <cstdint>
#include <string>

#include <sys/types.h>
#include <netinet/ip.h>

class UDPClient: public INetworkClient
{
public:
  UDPClient(const std::string& addr, int port);
  ~UDPClient() override = default;
  bool connect(bool blocking = true) override;
  bool disconnect() override;
  bool setDestination(const std::string& address, uint16_t port) override;
  int send(const uint8_t* data, size_t length) override;
  int receive(uint8_t* data, size_t length) override;
  bool isConnected() const override;

private:
  Logger logger_;
  std::string addr_;
  int port_;
  int sockfd_;
  struct sockaddr_in servaddr_;
};
