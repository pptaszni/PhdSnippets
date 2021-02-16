/**
 * /author Pawel Ptasznik
 */

#pragma once

#include "AsyncFrameListener.hpp"

#include <gmock/gmock.h>

class AsyncFrameListenerMock : public AsyncFrameListener {
 public:
  AsyncFrameListenerMock(): AsyncFrameListener(nullptr, 0) {}
  MOCK_METHOD1(setOnFrameCallback,
      void(Callback cb));
  MOCK_METHOD0(start,
      bool());
  MOCK_METHOD0(stop,
      bool());
  MOCK_METHOD1(requestRoi,
      void(const ROI& roi));
};
