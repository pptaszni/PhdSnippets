#pragma once

/**
 * @Author Pawel Ptasznik
 *
 * @Copyright Terabee 2019
 *
 */

#include <memory>
#include <string>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

struct LoggerParameterInitializer
{
  LoggerParameterInitializer();
};

class Logger
{
public:
  typedef std::shared_ptr<spdlog::logger> loggerPtr;
  Logger() = delete;
  Logger(const Logger& l) = default;
  explicit Logger(const std::string& name);
  loggerPtr operator->() const {
    return logger_;
  }
 private:
  static const LoggerParameterInitializer initializer_;
  loggerPtr logger_;
};
