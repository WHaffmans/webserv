#include <webserv/log/Channel.hpp>
#include <sstream>
#include <iomanip>

Log::Level Channel::getLogLevel() const
{
    return logLevel_;
}

void Channel::setLogLevel(Log::Level level)
{
    logLevel_ = level;
}

std::string Channel::printContext(const std::map<std::string, std::string> &context)
{
    std::stringstream ss;
    if (!context.empty())
    {
        bool first = true;
        for (const auto &[key, value] : context)
        {
            if (!first)
            {

                ss << "\n";
            }
            ss << "\t| " << std::setw(15) << std::right << std::setfill(' ') << key << " : " << value;

            first = false;
        }
        ss << "\n\n";

    }
    
    return ss.str();
}