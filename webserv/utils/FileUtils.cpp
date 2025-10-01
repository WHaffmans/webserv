#include <webserv/utils/FileUtils.hpp>

#include <cstring> // for strlen
#include <string>  // for string

#include <sys/stat.h> // for stat, S_ISREG, S_ISDIR

namespace FileUtils
{
bool isDirectory(const std::string &path)
{
    struct stat pathStat{};
    if (stat(path.c_str(), &pathStat) != 0)
    {
        return false; // Could not access path
    }
    return S_ISDIR(pathStat.st_mode);
}

bool isFile(const std::string &path)
{
    struct stat pathStat{};
    if (stat(path.c_str(), &pathStat) != 0)
    {
        return false; // Could not access path
    }
    return S_ISREG(pathStat.st_mode);
}

std::string getExtension(const std::string &filename)
{
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == filename.length() - 1)
    {
        return ""; // No extension found
    }
    return filename.substr(dotPos + 1);
}

std::string joinPath(const std::string &base, const std::string &addition) // NOLINT
{
    std::string result = base;
    if (result.empty())
    {
        result = addition;
    }
    else if (result.back() == '/' && addition.front() == '/')
    {
        result += addition.substr(1);
    }
    else if (result.back() != '/' && addition.front() != '/')
    {
        result += '/' + addition;
    }
    else
    {
        result += addition;
    }
    return result;
}
} // namespace FileUtils