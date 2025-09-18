#pragma once

#include <cstdint>
#include <string>

enum class LogLevel : uint8_t
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

inline std::string logLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARN:
        return "WARN";
    case LogLevel::ERROR:
        return "ERROR";
    }
    return "UNKNOWN"; // Fallback to silence warnings
}