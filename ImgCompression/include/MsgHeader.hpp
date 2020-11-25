/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <cstdint>

struct MsgHeader
{
  uint16_t payload;
  uint16_t width;
  uint16_t height;
  uint8_t crc;
  uint8_t calculateCrc() { return (payload + width + height) % 255; }
};
