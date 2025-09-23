#include <webserv/log/Channel.hpp>

#include <iomanip>
#include <sstream>

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
        for (const auto &[key, value] : context)
        {
            if (value.find('\n') != std::string::npos)
            {

                ss << "\t| " << std::right << std::setfill(' ') << "\e[1m" << key << "\e[0m" << " : \n";
                    std::istringstream valueStream(value);
                    std::string line;
                    while (std::getline(valueStream, line))
                    {
                        ss << std::setw(10) << "\t\t| " << line << "\n";
                    }
                continue;
            }
            ss << "\t| " << std::right << std::setfill(' ') << "\e[1m" << key << "\e[0m" << " : " << value << "\n";
        }
        ss << "\n";
    }

    return ss.str();
}