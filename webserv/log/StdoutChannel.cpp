#include "webserv/log/StdoutChannel.hpp"

#include <webserv/log/Channel.hpp>
#include <webserv/log/Log.hpp>

#include <iomanip>
#include <iostream>
#include <map>

StdoutChannel::StdoutChannel(Log::Level logLevel)
{
    setLogLevel(logLevel);
}

void StdoutChannel::log(const Log::Level &logLevel, const std::string &message,
                        const std::map<std::string, std::string> &context)
{
    if (logLevel < getLogLevel())
    {
        return;
    }
    std::cout << "[" << std::setw(3) << std::setfill('0') << Log::getElapsedTime() << "] ";
    std::string prefix = "[" + Log::logLevelToColoredString(logLevel) + "] ";
    std::cout << prefix;
    std::cout << message;
    if (!context.empty())
    {
        std::cout << "\n\t| Context: {";
        bool first = true;
        for (const auto &[key, value] : context)
        {
            if (!first)
            {
                std::cout << ", ";
            }
            std::cout << key << ": " << value;
            first = false;
        }
        std::cout << "}\n";
    }
    std::cout << "\n" << std::flush;
}