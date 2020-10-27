/**
 * /author Pawel Ptasznik
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>

class IClock
{
public:
    virtual ~IClock() = default;
    virtual std::chrono::time_point<std::chrono::steady_clock> now() = 0;
    virtual void sleepFor(std::chrono::milliseconds ms) = 0;
};

class ClockMock : public IClock {
 public:
  MOCK_METHOD0(now,
      std::chrono::time_point<std::chrono::steady_clock>());
  MOCK_METHOD1(sleepFor,
      void(std::chrono::milliseconds ms));
};

using testing::_;
using testing::Invoke;

class MyFixture: public ::testing::Test
{
 protected:
  MyFixture(): _clockMock(new ClockMock)
  {
    ON_CALL(*_clockMock, now()).WillByDefault(Invoke([]() { return std::chrono::steady_clock::now(); }));
    using namespace std::chrono_literals;
    ON_CALL(*_clockMock, sleepFor(_)).WillByDefault(Invoke([](auto) { std::this_thread::sleep_for(1ms); }));
  }
  std::shared_ptr<ClockMock> _clockMock;
};

TEST_F(MyFixture, xxx)
{
}

TEST_F(MyFixture, yyy)
{
}
