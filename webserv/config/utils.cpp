
#include <webserv/config/utils.hpp>

#include <stdexcept>
#include <string>

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

std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    size_t last = str.find_last_not_of(" \t\n\r");
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
} // namespace utils
