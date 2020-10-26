/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <cstdint>
#include <string>

class INetworkServer {
 public:
    virtual ~INetworkServer() = default;
    virtual bool acceptConnection(bool blocking = true) = 0;
    virtual bool disconnect() = 0;
    virtual bool setPort(uint16_t port) = 0;
    virtual int send(const uint8_t* data, size_t length) = 0;
    virtual int receive(uint8_t* data, size_t length) = 0;
    virtual bool isConnected() = 0;
};
