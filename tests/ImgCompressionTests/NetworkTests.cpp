/**
 * /author Pawel Ptasznik
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "UDPClient.hpp"
#include "UDPServer.hpp"

#include <future>
#include <memory>

class NetworkFixture: public ::testing::Test
{
 protected:
  NetworkFixture()
  {
    client_.reset(new UDPClient("127.0.0.1", 8080));
    server_.reset(new UDPServer(8080));
  }

  std::unique_ptr<INetworkClient> client_;
  std::unique_ptr<INetworkServer> server_;

};

TEST_F(NetworkFixture, sendAndReceiveDataCorrectly)
{
  std::string expectedMsg("My name is Tony");
  std::string expectedRep("Fuck you Tony!");
  size_t msgLen = expectedMsg.size();
  size_t repLen = expectedRep.size();
  ASSERT_TRUE(client_->connect());
  auto f = std::async(std::launch::async, [this, msgLen, expectedRep]()
    {
      server_->acceptConnection();
      std::unique_ptr<uint8_t[]> buff(new uint8_t[msgLen]);
      server_->receive(buff.get(), msgLen);
      server_->send(reinterpret_cast<const uint8_t*>(expectedRep.data()), expectedRep.size());
      return std::string((char*)buff.get(), msgLen);
    });
  client_->send(reinterpret_cast<const uint8_t*>(expectedMsg.data()), msgLen);
  std::unique_ptr<uint8_t[]> buff(new uint8_t[repLen]);
  client_->receive(buff.get(), repLen);
  std::string reply((char*)buff.get(), repLen);
  std::string result = f.get();
  ASSERT_EQ(expectedMsg, result);
  ASSERT_EQ(expectedRep, reply);
}
