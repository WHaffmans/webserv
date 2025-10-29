#pragma once

#include <array>         // for array
#include <chrono>        // for steady_clock
#include <cstdint>       // for uint8_t
#include <ios>           // for ios_base
#include <map>           // for map
#include <memory>        // for unique_ptr
#include <string>        // for string, basic_string, hash
#include <string_view>   // for string_view
#include <unordered_map> // for unordered_map

class Channel; // Forward declaration

constexpr const char *extractFilename(const char *path)
{
    const char *filename = path;
    while (*path != '\0')
    {
        if (*path == '/' || *path == '\\')
        {
            filename = path + 1; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
        ++path; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
    return filename;
}

#define LOCATION                                                                                                       \
    (std::string(extractFilename(__FILE__)) + ":" + std::to_string(__LINE__) + " (" + std::string(__FUNCTION__) + ")")

class Log
{
  public:
    Log(const Log &other) = delete;
    Log(Log &&other) = delete;
    Log &operator=(const Log &other) = delete;
    Log &operator=(Log &&other) = delete;

    enum class Level : uint8_t
    {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Fatal = 5
    };

    void log(Level level, const std::string &message, const std::map<std::string, std::string> &context);

    static constexpr Log::Level COMPILE_TIME_LOG_LEVEL = Log::Level::Trace;

    static void setFileChannel(const std::string &filename);
    static void setStdoutChannel();

    static int getElapsedTime();

    static void trace(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void debug(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void info(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void warning(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void error(const std::string &message, const std::map<std::string, std::string> &context = {});
    static void fatal(const std::string &message, const std::map<std::string, std::string> &context = {});

    static void status(const std::string &message);
    static void clearStatus();

    static std::string logLevelToString(Level level);
    static const char *logLevelToColor(Level level);
    static std::string logLevelToColoredString(Level level);
    static Level stringToLogLevel(const std::string &level);

    static void clearChannels();

  private:
    static std::string _statusMessage;
    static bool _statusActive;
    Log();

    ~Log() = default;
    static Log &getInstance();

    std::chrono::steady_clock::time_point start_time_;
    std::unordered_map<std::string, std::unique_ptr<Channel>> channels_;

    struct LevelMapping
    {
        Level level;
        std::string_view name;
        const char *color;
    };

    constexpr static std::array<LevelMapping, 6> LOG_LEVEL_MAP
        = {{{.level = Log::Level::Trace, .name = "TRACE", .color = "\033[36m"},
            {.level = Log::Level::Debug, .name = "DEBUG", .color = "\033[90m"},
            {.level = Log::Level::Info, .name = "INFO", .color = "\033[37m"},
            {.level = Log::Level::Warn, .name = "WARN", .color = "\033[33m"},
            {.level = Log::Level::Error, .name = "ERROR", .color = "\033[31m"},
            {.level = Log::Level::Fatal, .name = "FATAL", .color = "\033[1;31m"}}};

    constexpr static const char *RESET_COLOR = "\033[0m";
};
