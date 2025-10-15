#include "webserv/log/Log.hpp"

#include <webserv/config/AConfig.hpp>        // for AConfig
#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/config/ServerConfig.hpp>   // for ServerConfig
#include <webserv/handler/URI.hpp>
#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders
#include <webserv/utils/FileUtils.hpp>  // for joinPath, getExtension, isDirectory, isFile, isValidPath
#include <webserv/utils/utils.hpp>      // for trim, split

#include <cstddef>  // for size_t
#include <optional> // for optional, operator!=
#include <vector>   // for vector

URI::URI(const HttpRequest &request, const ServerConfig &serverConfig)
    : uriTrimmed_(utils::trim(request.getTarget(), "/")), config_(matchConfig(uriTrimmed_, serverConfig))
{
    Log::trace(LOCATION);
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

std::map<std::string, std::string> URI::getCGIEnvironment() const
{
    std::map<std::string, std::string> env;

    // URI components
    env["REQUEST_URI"] = uriTrimmed_;
    env["SCRIPT_NAME"] = getFullPath();
    env["PATH_INFO"] = getPathInfo();
    env["QUERY_STRING"] = getQuery();

    // Authority components
    env["SERVER_NAME"] = config_->get<std::string>("server_name").value_or("");
    env["SERVER_PORT"] = std::to_string(config_->get<int>("listen").value_or(-1));
    env["REQUEST_SCHEME"] = "HTTP";

    // HTTP context
    // env["REQUEST_METHOD"] = requestMethod_;
    // env["CONTENT_TYPE"] = contentType_;
    // env["CONTENT_LENGTH"] = contentLength_;

    return env;
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

bool URI::isCgi() const
{
    return !getCgiPath().empty();
}

std::string URI::getCgiPath() const
{
    // Log::debug("BaseName: " + baseName_ + ", FullPath: " + fullPath_ + ", Dir: " + dir_ + ", PathInfo: " + pathInfo_ +
    //            ", Extension: " + getExtension());
    if (!isFile() || getExtension().empty() || !config_->get<bool>("cgi_enabled").has_value()
        || !config_->get<bool>("cgi_enabled").value())
    {
        // Log::debug("CGI not enabled or not a file or no extension",
        //            {{"isFile", isFile() ? "true" : "false"},
        //             {"extension", getExtension()},
        //             {"cgi_enabled", config_->get<bool>("cgi_enabled").has_value() ? "true" : "false"},
        //             {"cgi_enabled_value", config_->get<bool>("cgi_enabled").value() ? "true" : "false"}});
        return "";
    }
    auto cgiPath = config_->getCGIPath(getExtension());
    return cgiPath;
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

const std::string &URI::getAuthority() const
{
    return authority_;
}