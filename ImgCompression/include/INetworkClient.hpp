/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <cstdint>
#include <string>

class INetworkClient
{
public:
    virtual ~INetworkClient() = default;
    virtual bool connect(bool blocking = true) = 0;
    virtual bool disconnect() = 0;
    virtual bool setDestination(const std::string& address, uint16_t port) = 0;
    virtual int send(const uint8_t* data, size_t length) = 0;
    virtual int receive(uint8_t* data, size_t length) = 0;
    virtual bool isConnected() const = 0;
};
