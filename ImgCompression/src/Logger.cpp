/**
 * @Author Pawel Ptasznik
 *
 */

#include "Logger.hpp"

#include <cstdlib>
#include <memory>
#include <string>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#define LOGGER_FILENAME "TerabeeLog.log"
#define LOGGER_MAX_FILE_SIZE 1024*1024*64  // using 64MB log files
#define LOGGER_MAX_NUMBER_OF_FILES 20  // let's say that 20 files (64MB each) is enough

namespace
{
std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> g_file_sink;
}

LoggerParameterInitializer::LoggerParameterInitializer() {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] <PID:%P> <Thread:%t> [%l] [%n] : %v%$");
}

Logger::Logger(const std::string& name) {
  logger_ = spdlog::get(name);
  if (logger_) return;
  logger_ = spdlog::stdout_color_mt(name);
}

const LoggerParameterInitializer Logger::initializer_ = LoggerParameterInitializer();
