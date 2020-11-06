/**
 * /author Pawel Ptasznik
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <thread>
#include <iostream>

#include <QJsonObject>
#include <QDebug>

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
  EXPECT_CALL(*_clockMock, now()).WillRepeatedly(Invoke([]() {
    throw std::runtime_error("xxx");
    return std::chrono::steady_clock::now();
  }));
  try
  {
    _clockMock->now();
  }
  catch(...)
  {
    std::cout << "got exception" << std::endl;
  }
}

TEST_F(MyFixture, yyy)
{
  QJsonObject obj1({{"bla1", "lab1"}});
  QJsonObject obj2({{"bla2", "lab2"}});
  QJsonObject obj34({{"bla3", "lab3"}, {"bla4", "lab4"}});
  QJsonObject result;
  for (const auto& json : {obj1, obj2, obj34})
  {
    for (auto it = json.begin(); it != json.end(); it++)
    {
      result.insert(it.key(), it.value());
    }
  }
  for (auto it = result.begin(); it != result.end(); it++)
  {
    qDebug() << it.key() << ": " << it.value();
  }
}
