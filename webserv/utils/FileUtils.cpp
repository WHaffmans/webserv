#include <webserv/utils/FileUtils.hpp>

#include <webserv/log/Log.hpp> // for Log, LOCATION

#include <cstring> // for size_t
#include <fstream> // for basic_ifstream, basic_ios, basic_istream, ios, ifstream, operator|, basic_istream::read, basic_istream::seekg, basic_istream::tellg, streamsize
#include <iterator> // for istreambuf_iterator, operator==
#include <string>   // for basic_string, string, char_traits, operator+

#include <sys/stat.h> // for stat, S_ISDIR, S_ISREG

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

bool isValidPath(const std::string &path)
{
    struct stat pathStat{};
    return stat(path.c_str(), &pathStat) == 0;
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

std::vector<char> readBinaryFile(const std::string &filepath)
{
    Log::trace(LOCATION);

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        Log::error("Failed to open file: " + filepath);
        return {};
    }

    std::streamsize size = file.tellg();
    if (size < 0)
    {
        Log::error("Failed to determine file size: " + filepath);
        return {};
    }
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(static_cast<size_t>(size));
    if (!file.read(buffer.data(), size))
    {
        Log::error("Failed to read file: " + filepath);
        return {};
    }

    return buffer;
}

std::string readFileAsString(const std::string &filepath)
{
    Log::trace(LOCATION);

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        return "";
    }

    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

} // namespace FileUtils