#include "webserv/config/AConfig.hpp"
#include "webserv/utils/FileUtils.hpp"
#include "webserv/utils/utils.hpp"

#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/config/ServerConfig.hpp>   // for ServerConfig
#include <webserv/handler/URI.hpp>

#include <optional> // for optional

#include <stddef.h>   // for size_t
#include <sys/stat.h> // for stat, S_ISDIR, S_ISREG

URI::URI(const HttpRequest &request, const ServerConfig &serverConfig)
    : uriTrimmed_(utils::trim(request.getTarget(), "/")), config_(matchConfig(uriTrimmed_, serverConfig))
{
    parseUri(request.getTarget());
    parseFullpath();

    authority_ = request.getHeaders().getHost().value();
    authority_ += (serverConfig.get<int>("listen") != 80) // NOFORMAT
                      ? ":" + std::to_string(serverConfig.get<int>("listen").value())
                      : "";
}

const AConfig *URI::matchConfig(const std::string &uri, const ServerConfig &serverConfig)
{
    const auto &locations = serverConfig.getLocationPaths();
    const AConfig *bestMatch = &serverConfig;

    size_t maxMatchLength = 0;
    for (const auto &locationPath : locations)
    {
        if (uri.empty() && locationPath == "/")
        {
            return serverConfig.getLocation(locationPath);
        }
        if (uri.starts_with(utils::trim(locationPath, "/")))
        {
            if (locationPath.length() > maxMatchLength)
            {
                maxMatchLength = locationPath.length();
                bestMatch = serverConfig.getLocation(locationPath);
            }
        }
    }
    return bestMatch;
}

void URI::parseUri(const std::string &uri)
{
    if (config_->getType() == "server")
    {
        fullPath_ = FileUtils::joinPath(config_->get<std::string>("root").value_or(""), uriTrimmed_);
    }
    else
    {
        auto const *locConfig = dynamic_cast<LocationConfig const *>(config_);
        std::string locTrimmed = utils::trim(locConfig->getPath(), "/");
        std::string uriSub = uri.substr(locTrimmed.length());
        fullPath_ = FileUtils::joinPath(locConfig->get<std::string>("root").value_or(""), uriSub);
    }

    size_t fragmentPos = fullPath_.find_first_of('#');
    if (fragmentPos != std::string::npos)
    {
        fragment_ = fullPath_.substr(fragmentPos + 1);
        fullPath_ = fullPath_.substr(0, fragmentPos);
    }

    size_t queryPos = fullPath_.find_first_of('?');
    if (queryPos != std::string::npos)
    {
        query_ = fullPath_.substr(queryPos + 1);
        fullPath_ = fullPath_.substr(0, queryPos);
    }
}

void URI::parseFullpath()
{
    auto uriSegments = utils::split(fullPath_, '/');

    for (const auto &segment : uriSegments)
    {
        std::string curDir = FileUtils::joinPath(dir_, segment);
        if (segment.empty())
        {
            continue;
        }

        if (FileUtils::isFile(curDir) && baseName_.empty())
        {
            baseName_ = segment;
        }
        else if (FileUtils::isDirectory(curDir))
        {
            dir_ = FileUtils::joinPath(dir_, segment);
        }
        else if (!baseName_.empty()) // not file or dir, but we have a baseName already
        {
            pathInfo_ = FileUtils::joinPath(pathInfo_, baseName_);
        }
    }
    fullPath_ = FileUtils::joinPath(dir_, baseName_);
}

const AConfig *URI::getConfig() const
{
    return config_;
}

bool URI::isFile() const
{
    return !baseName_.empty();
}

bool URI::isDirectory() const
{
    return baseName_.empty();
}

bool URI::isValid() const
{
    return FileUtils::isValidPath(fullPath_);
}

const std::string &URI::getBaseName() const
{
    return baseName_;
}

std::string URI::getExtension() const
{
    return FileUtils::getExtension(baseName_);
}

const std::string &URI::getFullPath() const
{
    return fullPath_;
}

const std::string &URI::getDir() const
{
    return dir_;
}

const std::string &URI::getPathInfo() const
{
    return pathInfo_;
}

const std::string &URI::getQuery() const
{
    return query_;
}

const std::string &URI::getFragment() const
{
    return fragment_;
}