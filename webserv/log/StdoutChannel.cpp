#include <webserv/log/StdoutChannel.hpp>

#include <iomanip>
#include <iostream>
#include <map>

void StdoutChannel::log(LogLevel &logLevel, const std::string &message,
                        const std::map<std::string, std::string> &context)
{
    std::cout << "[" << std::setw(3) << std::setfill('0') << Log::getElapsedTime() << "] ";
    std::string prefix = "[" + logLevelToColoredString(logLevel) + "] ";
    std::cout << prefix;
    std::cout << message;
    if (!context.empty())
    {
        std::cout << " Context: {";
        for (const auto &[key, value] : context)
        {
            std::cout << key << ": " << value << ", ";
        }
        std::cout << "}";
    }
    std::cout << "\n";
}