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
    : uriTrimmed_(utils::uriDecode(utils::trim(request.getTarget(), "/"))),
      config_(matchConfig(uriTrimmed_, serverConfig))
{
    Log::trace(LOCATION);
    parseUri();
    Log::debug("Parsed URI: " + uriTrimmed_, {{"ConfigType", config_->getType()}});
    parseFullpath();

    authority_ = request.getHeaders().getHost().value();
}

const AConfig *URI::matchConfig(const std::string &uri, const ServerConfig &serverConfig)
{
    const auto &locations = serverConfig.getLocationPaths();
    const AConfig *matchedConfig = &serverConfig;

    size_t maxMatchLength = 0;
    for (const auto &location : locations)
    {
        if (location == "/")
        {
            if (uri.empty())
            {
                return serverConfig.getLocation(location);
            }
            // Root location matches everything, but with lowest priority
            if (maxMatchLength == 0)
            {
                matchedConfig = serverConfig.getLocation(location);
                maxMatchLength = 1;
            }
            continue;
        }

        // Trim leading/trailing slashes for consistent matching
        std::string trimmedLocation = utils::trim(location, "/");

        // Check if URI starts with the location path
        if (!uri.starts_with(trimmedLocation))
        {
            continue;
        }

        // Ensure we have an exact path boundary match
        // Either the URI is exactly the location, or the next character is '/'
        size_t queryPos = uri.find_first_of('?');
        auto checkUri = (queryPos != std::string::npos) ? uri.substr(0, queryPos) : uri;
        if (checkUri.length() > trimmedLocation.length() && checkUri[trimmedLocation.length()] != '/')
        {
            continue; // Not a path boundary match
        }

        // This is a valid match - check if it's the longest so far
        if (trimmedLocation.length() > maxMatchLength)
        {
            maxMatchLength = trimmedLocation.length();
            matchedConfig = serverConfig.getLocation(location);
        }
    }
    return matchedConfig;
}

void URI::parseUri()
{
    if (config_->getType() == "server")
    {
        fullPath_ = FileUtils::joinPath(config_->get<std::string>("root").value_or(""), uriTrimmed_);
    }
    else
    {
        auto const *locConfig = dynamic_cast<LocationConfig const *>(config_);
        std::string locTrimmed = utils::trim(locConfig->getPath(), "/");

        std::string uriSub = uriTrimmed_.substr(locTrimmed.length());
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
    if (fullPath_.front() == '/')
    {
        dir_ = "/";
    }
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
            isDir_ = false;
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
            if (config_->get<bool>("42_tester").value_or(false))
            {
                baseName_ = segment;
            }
            Log::debug("Invalid path segment encountered: " + currentPath);
            return;
        }
    }
    if (baseName_.empty() && FileUtils::isDirectory(fullPath_))
    {
        std::string index = config_->get<std::string>("index").value_or("");
        std::string indexPath = FileUtils::joinPath(fullPath_, index);
        if (FileUtils::isFile(indexPath))
        {
            baseName_ = index;
        }
    }
    Log::debug("URI parseFullpath results", {{"dir", dir_}, {"baseName", baseName_}, {"pathInfo", pathInfo_}});
    if (baseName_.empty())
    {
        fullPath_ = dir_ + "/";
    }
    else
    {
        fullPath_ = FileUtils::joinPath(dir_, baseName_);
    }
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
    if (config_->get<bool>("42_tester"))
    {
        return baseName_.empty();
    }
    return isDir_;
}

bool URI::isValid() const noexcept
{
    return valid_ && FileUtils::isValidPath(fullPath_);
}

bool URI::isCgi() const noexcept
{
    Log::debug("Check for CGI extension: " + getExtension() + ", with basename " + baseName_);
    if (config_->get<bool>("42_tester"))
    {
        return config_->isCGI(getExtension());
    }
    return config_->isCGI(getExtension()) && FileUtils::isFile(fullPath_);
}

bool URI::isRedirect() const noexcept
{
    auto redirectOpt = config_->get<std::pair<int, std::string>>("redirect");
    return redirectOpt.has_value();
}

bool URI::isUpload() const noexcept
{
    auto uploadStoreOpt = config_->get<std::string>("upload_store");
    return uploadStoreOpt.has_value();
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

    Log::debug(
        "Generating URI for path",
        {{"path", path}, {"trimmedDir", trimmedLocation}, {"trimmedPath", trimmedPath}, {"Authority", authority_}});

    std::string result = std::string(Http::Protocol::HTTP_SCHEME) + "://" + authority_;
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