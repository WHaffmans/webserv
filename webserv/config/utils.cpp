#include <webserv/config/utils.hpp>

#include <string>

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
    return std::string::npos; // No matching closing brace found
}