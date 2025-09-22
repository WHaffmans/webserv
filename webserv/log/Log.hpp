#pragma once

// #include <webserv/log/Channel.hpp>

#include <array>
#include <chrono>
#include <map>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <unordered_map>

class Channel; // Forward declaration

class Log
{
  public:
    enum class Level : uint8_t
    {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Fatal = 5
    };

    Log(const Log &other) = delete;
    Log(const Log &&other) = delete;
    Log &operator=(const Log &other) = delete;
    Log &&operator=(const Log &&other) = delete;

    void log(Level level, const std::string &message, const std::map<std::string, std::string> &context,
             const std::source_location &location);

    static void setFileChannel(const std::string &filename, std::ios_base::openmode mode = std::ios_base::app,
                               Level logLevel = Level::Trace);
    static void setStdoutChannel(Level logLevel = Level::Trace);

    static int getElapsedTime();

    static void trace(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());
    static void debug(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());
    static void info(const std::string &message, const std::map<std::string, std::string> &context = {},
                     const std::source_location &location = std::source_location::current());
    static void warning(const std::string &message, const std::map<std::string, std::string> &context = {},
                        const std::source_location &location = std::source_location::current());
    static void error(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());
    static void fatal(const std::string &message, const std::map<std::string, std::string> &context = {},
                      const std::source_location &location = std::source_location::current());
    static std::string logLevelToString(Level level);
    static const char *logLevelToColor(Level level);
    static std::string logLevelToColoredString(Level level);
    static Log::Level stringToLogLevel(const std::string &level);

  private:
    Log();

    ~Log() = default;
    static Log &getInstance();

    std::chrono::steady_clock::time_point start_time_;
    std::unordered_map<std::string, std::unique_ptr<Channel>> channels_;



    struct LogLevelMapping
    {
        Log::Level level;
        std::string_view name;
        const char *color;
    };

    constexpr static std::array<LogLevelMapping, 6> LOG_LEVEL_MAP = {
        {{.level = Log::Level::Trace, .name = "TRACE", .color = "\033[36m"},
         {.level = Log::Level::Debug, .name = "DEBUG", .color = "\033[90m"},
         {.level = Log::Level::Info, .name = "INFO", .color = "\033[37m"},
         {.level = Log::Level::Warn, .name = "WARN", .color = "\033[33m"},
         {.level = Log::Level::Error, .name = "ERROR", .color = "\033[31m"},
         {.level = Log::Level::Fatal, .name = "FATAL", .color = "\033[1;31m"}}};
};
