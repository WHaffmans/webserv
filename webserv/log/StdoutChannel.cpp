#include <iostream>
#include <map>
#include <webserv/log/StdoutChannel.hpp>

void StdoutChannel::log(LogLevel &logLevel, const std::string &message,
                        const std::map<std::string, std::string> &context)
{
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