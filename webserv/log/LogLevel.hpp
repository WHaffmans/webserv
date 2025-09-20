#pragma once

#include <cstdint>
#include <string>

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

inline std::string logLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::LOGLVL_TRACE: return "TRACE";
    case LogLevel::LOGLVL_DEBUG: return "DEBUG";
    case LogLevel::LOGLVL_INFO: return "INFO";
    case LogLevel::LOGLVL_WARN: return "WARN";
    case LogLevel::LOGLVL_ERROR: return "ERROR";
    case LogLevel::LOGLVL_FATAL: return "FATAL";
    }
    return "UNKNOWN";
}

inline const char *logLevelToColor(LogLevel level)
{
    switch (level)
    {
    case LogLevel::LOGLVL_TRACE: return LogColors::TRACE_COLOR;
    case LogLevel::LOGLVL_DEBUG: return LogColors::DEBUG_COLOR;
    case LogLevel::LOGLVL_INFO: return LogColors::INFO_COLOR;
    case LogLevel::LOGLVL_WARN: return LogColors::WARN_COLOR;
    case LogLevel::LOGLVL_ERROR: return LogColors::ERROR_COLOR;
    case LogLevel::LOGLVL_FATAL: return LogColors::FATAL_COLOR;
    }
    return LogColors::RESET;
}

inline std::string logLevelToColoredString(LogLevel level)
{
    return std::string(logLevelToColor(level)) + logLevelToString(level) + LogColors::RESET;
}

inline LogLevel stringToLogLevel(const std::string &level)
{
    if (level == "TRACE")
    {
        return LogLevel::LOGLVL_TRACE;
    }
    if (level == "DEBUG")
    {
        return LogLevel::LOGLVL_DEBUG;
    }
    if (level == "INFO")
    {
        return LogLevel::LOGLVL_INFO;
    }
    if (level == "WARN")
    {
        return LogLevel::LOGLVL_WARN;
    }
    if (level == "ERROR")
    {
        return LogLevel::LOGLVL_ERROR;
    }
    if (level == "FATAL")
    {
        return LogLevel::LOGLVL_FATAL;
    }
    return LogLevel::LOGLVL_INFO; // Default fallback
}