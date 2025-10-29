#include <webserv/config/AConfig.hpp>        // for AConfig
#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/config/ServerConfig.hpp>   // for ServerConfig
#include <webserv/handler/URI.hpp>
#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders
#include <webserv/log/Log.hpp>          // for Log, LOCATION
#include <webserv/utils/FileUtils.hpp>  // for joinPath, isDirectory, isFile, getExtension, isValidPath
#include <webserv/utils/utils.hpp>      // for trim, split

#include <cstddef>  // for size_t
#include <map>      // for map
#include <optional> // for optional, operator!=
#include <vector>   // for vector

URI::URI(const HttpRequest &request, const ServerConfig &serverConfig)
    : uriTrimmed_(utils::trim(request.getTarget(), "/")), config_(matchConfig(uriTrimmed_, serverConfig))
{
    Log::trace(LOCATION);
    parseUri(request.getTarget());
    Log::debug("Parsed URI: " + uriTrimmed_, {{"ConfigType", config_->getType()}});
    parseFullpath();

    authority_ = request.getHeaders().getHost().value();
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
        std::string uriTrimmed = utils::trim(uri, "/");

        std::string uriSub = uriTrimmed.substr(locTrimmed.length());
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
        std::string currentPath = FileUtils::joinPath(dir_, segment);
        if (segment.empty())
        {
            continue;
        }

        if (FileUtils::isFile(currentPath) && baseName_.empty())
        {
            baseName_ = segment;
        }
        else if (FileUtils::isDirectory(currentPath))
        {
            dir_ = FileUtils::joinPath(dir_, segment);
        }
        else if (!baseName_.empty()) // not file or dir, but we have a baseName already
        {
            if (pathInfo_.empty())
            {
                pathInfo_ = "/";
            }

            pathInfo_ = FileUtils::joinPath(pathInfo_, segment);
        }
        else // not file or dir, and no baseName yet
        {
            valid_ = false;
            Log::warning("Invalid path segment encountered: " + currentPath);
            return;
        }
    }
    if (baseName_.empty() && FileUtils::isDirectory(fullPath_))
    {
        for (const auto &index : config_->get<std::vector<std::string>>("index").value_or(std::vector<std::string>()))
        {
            std::string indexPath = FileUtils::joinPath(fullPath_, index);
            if (FileUtils::isFile(indexPath))
            {
                baseName_ = index;
                break;
            }
        }
    }
    Log::debug("URI parseFullpath results", {{"dir", dir_}, {"baseName", baseName_}, {"pathInfo", pathInfo_}});
    fullPath_ = FileUtils::joinPath(dir_, baseName_);
}

const AConfig *URI::getConfig() const noexcept
{
    return config_;
}

bool URI::isFile() const noexcept
{
    return !baseName_.empty();
}

bool URI::isDirectory() const noexcept
{
    return baseName_.empty();
}

bool URI::isValid() const noexcept
{
    return valid_ && FileUtils::isValidPath(fullPath_);
}

bool URI::isCgi() const noexcept
{
    return !getCgiPath().empty();
}

bool URI::isRedirect() const noexcept
{
    auto redirectOpt = config_->get<std::pair<int, std::string>>("redirect");
    return redirectOpt.has_value();
}

std::pair<int, std::string> URI::getRedirect() const
{
    auto redirectOpt = config_->get<std::pair<int, std::string>>("redirect");
    if (redirectOpt.has_value())
    {
        return redirectOpt.value();
    }
    return {0, ""};
}

std::string URI::getCgiPath() const
{
    // Log::debug("BaseName: " + baseName_ + ", FullPath: " + fullPath_ + ", Dir: " + dir_ + ", PathInfo: " + pathInfo_
    // +
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

std::string URI::getUriForPath(const std::string &path) const
{
    // TOPD not good yet zo even naar kijken
    std::string trimmedPath = utils::trim(path, "/");
    std::string trimmedLocation;
    
    const auto *locConfig = dynamic_cast<const LocationConfig *>(config_);
    if (locConfig != nullptr)
    {
        trimmedLocation = utils::trim(locConfig->getPath(), "/");
    }

    
    std::string trimmedRoot = utils::trim(config_->get<std::string>("root").value_or(""), "/");

    if (trimmedPath.starts_with(trimmedRoot))
    {
        trimmedPath = trimmedPath.substr(trimmedRoot.length());
        trimmedPath = utils::trim(trimmedPath, "/");
    }

    Log::debug("Generating URI for path", {{"path", path},{"trimmedDir", trimmedLocation}, {"trimmedPath", trimmedPath}, {"Authority", authority_}});
    std::string result = "http://" + authority_; //TODO this should not be hardcoded...
    result = FileUtils::joinPath(result, trimmedLocation);
    return FileUtils::joinPath(result, trimmedPath);
}
    

const std::string &URI::getBaseName() const noexcept
{
    return baseName_;
}

std::string URI::getExtension() const noexcept
{
    return FileUtils::getExtension(baseName_);
}

const std::string &URI::getFullPath() const noexcept
{
    return fullPath_;
}

const std::string &URI::getDir() const noexcept
{
    return dir_;
}

const std::string &URI::getPathInfo() const noexcept
{
    return pathInfo_;
}

const std::string &URI::getQuery() const noexcept
{
    return query_;
}

const std::string &URI::getFragment() const noexcept
{
    return fragment_;
}

const std::string &URI::getAuthority() const noexcept
{
    return authority_;
}