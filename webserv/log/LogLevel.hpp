#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

// TODO LOG_LEVEL_MAP should not be in the global namespace, but i do think this is the nice way? lets refactor this to
// the log class or use a seperate namespace or put everything in a log namespace except for the class, then everything
// has Log::

enum class LogLevel : uint8_t
{
    LOGLVL_TRACE = 0,
    LOGLVL_DEBUG = 1,
    LOGLVL_INFO = 2,
    LOGLVL_WARN = 3,
    LOGLVL_ERROR = 4,
    LOGLVL_FATAL = 5
};

// ANSI color codes
namespace LogColors
{
constexpr const char *RESET = "\033[0m";
constexpr const char *TRACE_COLOR = "\033[36m";   // Cyan
constexpr const char *DEBUG_COLOR = "\033[90m";   // Bright black (gray)
constexpr const char *INFO_COLOR = "\033[37m";    // White
constexpr const char *WARN_COLOR = "\033[33m";    // Yellow
constexpr const char *ERROR_COLOR = "\033[31m";   // Red
constexpr const char *FATAL_COLOR = "\033[1;31m"; // Bold red
} // namespace LogColors

// Constexpr bidirectional mapping
struct LogLevelMapping
{
    LogLevel level;
    std::string_view name;
    const char *color;
};

constexpr std::array<LogLevelMapping, 6> LOG_LEVEL_MAP = {
    {{.level = LogLevel::LOGLVL_TRACE, .name = "TRACE", .color = LogColors::TRACE_COLOR},
     {.level = LogLevel::LOGLVL_DEBUG, .name = "DEBUG", .color = LogColors::DEBUG_COLOR},
     {.level = LogLevel::LOGLVL_INFO, .name = "INFO", .color = LogColors::INFO_COLOR},
     {.level = LogLevel::LOGLVL_WARN, .name = "WARN", .color = LogColors::WARN_COLOR},
     {.level = LogLevel::LOGLVL_ERROR, .name = "ERROR", .color = LogColors::ERROR_COLOR},
     {.level = LogLevel::LOGLVL_FATAL, .name = "FATAL", .color = LogColors::FATAL_COLOR}}};

inline std::string logLevelToString(LogLevel level)
{
    for (const auto &mapping : LOG_LEVEL_MAP)
    {
        if (mapping.level == level)
        {
            return std::string(mapping.name);
        }
    }
    return "UNKNOWN";
}

inline const char *logLevelToColor(LogLevel level)
{
    for (const auto &mapping : LOG_LEVEL_MAP)
    {
        if (mapping.level == level)
        {
            return mapping.color;
        }
    }
    return LogColors::RESET;
}

inline std::string logLevelToColoredString(LogLevel level)
{
    return std::string(logLevelToColor(level)) + logLevelToString(level) + LogColors::RESET;
}

inline LogLevel stringToLogLevel(const std::string &level)
{
    for (const auto &mapping : LOG_LEVEL_MAP)
    {
        if (mapping.name == level)
        {
            return mapping.level;
        }
    }
    return LogLevel::LOGLVL_INFO; // Default fallback
}