#include <webserv/log/Channel.hpp>

#include <iomanip> // for operator<<, setfill, setw
#include <sstream> // for basic_ostream, operator<<, basic_stringstream, basic_istream, basic_istringstream, right, istringstream, stringstream
#include <utility> // for get

std::string Channel::printContext(const std::map<std::string, std::string> &context)
{
    std::stringstream ss;
    if (!context.empty())
    {
        for (const auto &[key, value] : context)
        {
            if (value.find('\n') != std::string::npos)
            {

                ss << "\t| " << std::right << std::setfill(' ') << "\033[1m" << key << "\033[0m" << " : \n";
                std::istringstream valueStream(value);
                std::string line;
                while (std::getline(valueStream, line))
                {
                    ss << std::setw(10) << "\t\t| " << line << "\n";
                }
                continue;
            }
            ss << "\t| " << std::right << std::setfill(' ') << "\033[1m" << key << "\033[0m" << " : " << value << "\n";
        }
        ss << "\n";
    }

    return ss.str();
}

bool Channel::isStdOut() const
{
    return false;
}