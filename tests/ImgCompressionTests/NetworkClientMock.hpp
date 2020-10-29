/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "INetworkClient.hpp"

#include <gmock/gmock.h>

class NetworkClientMock : public INetworkClient {
 public:
  MOCK_METHOD1(connect,
      bool(bool));
  MOCK_METHOD0(disconnect,
      bool());
  MOCK_METHOD2(setDestination,
      bool(const std::string& address, uint16_t port));
  MOCK_METHOD2(send,
      int(const uint8_t* data, size_t length));
  MOCK_METHOD2(receive,
      int(uint8_t* data, size_t length));
  MOCK_CONST_METHOD0(isConnected,
      bool());
};
