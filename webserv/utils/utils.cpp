
#include <webserv/socket/ASocket.hpp>
#include <webserv/utils/utils.hpp>

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <sys/epoll.h>

namespace utils
{
size_t stoul(const std::string &str, int base)
{
    if (str.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos)
    {
        throw std::invalid_argument("Invalid number: " + str);
    }
    size_t idx = 0;
    unsigned long value = std::stoul(str, &idx, base);
    if (idx != str.length())
    {
        throw std::invalid_argument("Invalid number with extra characters: " + str);
    }
    return value;
}

std::string trim(const std::string &str, const std::string &charset)
{
    size_t first = str.find_first_not_of(charset);
    size_t last = str.find_last_not_of(charset);
    if (first == std::string::npos || last == std::string::npos)
    {
        return "";
    }
    return str.substr(first, last - first + 1);
}

std::string trimSemi(const std::string &str)
{
    size_t semi = str.find(';');

    if (semi == str.length() - 1)
    {
        return str.substr(0, semi);
    }
    return str;
}

size_t findCorrespondingClosingBrace(const std::string &str, size_t openPos)
{
    int braceCount = 1;
    if (str[openPos] != '{')
    {
        return std::string::npos; // Not an opening brace at the given position
    }

    for (size_t i = openPos + 1; i < str.size(); ++i)
    {
        if (str[i] == '{')
        {
            ++braceCount;
        }
        else if (str[i] == '}')
        {
            --braceCount;
        }

        if (braceCount == 0)
        {
            return i;
        }
    }
    return std::string::npos;
}

void removeEmptyLines(std::string &str)
{
    std::istringstream stream(str);
    std::string line;
    std::string result;

    while (std::getline(stream, line))
    {
        if (!utils::trim(line).empty())
        {
            result += utils::trim(line, " \t\n\r") + '\n';
        }
    }
    str = result;
}

void removeComments(std::string &str)
{
    size_t pos = 0;
    while ((pos = str.find('#', pos)) != std::string::npos)
    {
        size_t end = str.find('\n', pos);
        if (end == std::string::npos)
        {
            str.erase(pos);
        }
        else
        {
            str.erase(pos, end - pos);
        }
    }
    removeEmptyLines(str);
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> parts;
    std::string part;
    std::istringstream stream(str);

    while (std::getline(stream, part, delimiter))
    {
        parts.push_back(part);
    }
    return parts;
}

std::string implode(const std::vector<std::string> &elements, const std::string &delimiter)
{
    std::ostringstream stream;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        stream << elements[i];
        if (i < elements.size() - 1)
        {
            stream << delimiter;
        }
    }
    return stream.str();
}

uint32_t stateToEpoll(const ASocket::IoState &event)
{
    uint32_t epollEvents = 0;
    using EventType = std::underlying_type_t<ASocket::IoState>;
    if ((static_cast<EventType>(event) & static_cast<EventType>(ASocket::IoState::READ)) != 0U)
    {
        epollEvents |= EPOLLIN;
    }
    if ((static_cast<EventType>(event) & static_cast<EventType>(ASocket::IoState::WRITE)) != 0U)
    {
        epollEvents |= EPOLLOUT;
    }
    return epollEvents;
}

std::string uriEncode(const std::string &value)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value)
    {
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            escaped << c;
        }
        else
        {
            // Any other characters are percent-encoded
            escaped << '%' << std::uppercase << std::setw(2) << int(static_cast<unsigned char>(c)) << std::nouppercase;
        }
    }

    return escaped.str();
}

std::string uriDecode(const std::string &value)
{
    std::ostringstream unescaped;
    size_t length = value.length();

    for (size_t i = 0; i < length; ++i)
    {
        if (value[i] == '%' && i + 2 < length)
        {
            std::istringstream hexStream{value.substr(i + 1, 2)};
            int hexValue = 0;
            hexStream >> std::hex >> hexValue;

            // Reject null bytes
            if (hexValue == 0)
            {
                throw std::invalid_argument("Null byte in URI");
            }

            // Reject CR or LF to prevent header/request splitting through CGI parameters
            if (hexValue == 10 /*\n*/ || hexValue == 13 /*\r*/)
            {
                throw std::invalid_argument("CR/LF in URI not allowed");
            }

            // Don't decode slashes - they have special meaning in URIs
            if (hexValue == '/')
            {
                unescaped << value[i] << value[i + 1] << value[i + 2];
                i += 2;
                continue;
            }

            unescaped << static_cast<char>(hexValue);
            i += 2;
        }
        else
        {
            unescaped << value[i];
        }
    }

    return unescaped.str();
}

} // namespace utils