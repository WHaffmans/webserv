#include <webserv/handler/URIParser.hpp>

#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/config/ServerConfig.hpp>   // for ServerConfig

#include <optional> // for optional
#include <vector>   // for vector

#include <stddef.h>   // for size_t
#include <sys/stat.h> // for stat, S_ISDIR, S_ISREG

URIParser::URIParser(const std::string &uri, const ServerConfig &serverConfig) : _locationConfig(nullptr)
{
    const auto &locations = serverConfig.getLocationPaths();
    size_t maxMatchLength = 0;

    for (const auto &locationPath : locations)
    {
        if (uri.starts_with((locationPath == "/") ? locationPath : locationPath + "/"))
        { // TODO HMHMMz why does it need to end on a /?
            if (locationPath.length() > maxMatchLength)
            {
                maxMatchLength = locationPath.length();
                _locationConfig = serverConfig.getLocation(locationPath);
            }
        }
    }

    root_ = _locationConfig != nullptr ? _locationConfig->get<std::string>("root").value_or("") : "";
    if (!root_.empty() && root_.back() == '/')
    {
        root_.pop_back(); // Remove trailing slash to avoid double slashes in path
    }

    relativePath_ = uri.substr(maxMatchLength);
    if (relativePath_.empty() || relativePath_[0] != '/')
    {
        relativePath_ = "/" + relativePath_;
    }
}

std::string URIParser::getFilePath() const
{
    return root_ + relativePath_;
}

std::string URIParser::getFilename() const
{
    size_t lastSlash = relativePath_.find_last_of('/');
    if (lastSlash == std::string::npos)
    {
        return relativePath_; // No slashes, return the whole path
    }
    return relativePath_.substr(lastSlash + 1);
}

std::string URIParser::getExtension() const
{
    std::string filename = getFilename();
    size_t lastDot = filename.find_last_of('.');
    if (lastDot == std::string::npos || lastDot == 0 || lastDot == filename.length() - 1)
    {
        return ""; // No extension found or dot is at start/end
    }
    return filename.substr(lastDot + 1);
}

LocationConfig const *URIParser::getLocation() const
{
    return _locationConfig;
}

bool URIParser::isFile() const
{
    struct stat pathStat{};
    if (stat(getFilePath().c_str(), &pathStat) != 0)
    {
        return false;
    }
    return S_ISREG(pathStat.st_mode);
}

bool URIParser::isDirectory() const
{
    struct stat pathStat{};
    if (stat(getFilePath().c_str(), &pathStat) != 0)
    {
        return false;
    }
    return S_ISDIR(pathStat.st_mode);
}

bool URIParser::isValid() const
{
    struct stat pathStat{};
    return stat(getFilePath().c_str(), &pathStat) == 0;
}
