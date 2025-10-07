
#include <webserv/utils/utils.hpp>

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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

void removeEmptyLines(std::string &str)
{
    std::istringstream stream(str);
    std::string line;
    std::string result;

    while (std::getline(stream, line))
    {
        if (!utils::trim(line).empty())
        {
            result += utils::trimSemi(utils::trim(line)) + '\n';
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
} // namespace utils