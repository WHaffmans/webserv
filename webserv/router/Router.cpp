#include "webserv/config/AConfig.hpp"
#include "webserv/config/ConfigManager.hpp"
#include "webserv/config/ServerConfig.hpp"
#include "webserv/handler/ErrorHandler.hpp"
#include "webserv/http/HttpResponse.hpp"
#include "webserv/log/Log.hpp"

#include <webserv/router/Router.hpp>

#include <string>

Router::Router() {}

HttpResponse Router::handleRequest(const HttpRequest &request) const
{
    Log::trace(LOCATION);

    ServerConfig *config =
        ConfigManager::getInstance().getMatchingServerConfig(request.getHeaders().getHost().value_or(""));
    HttpResponse response;

    if (config == nullptr)
    {
        Log::warning("No matching server config found");
        handleError(400, response);
        return response;
    }

    const std::string &target = request.getTarget();
    const std::string &method = request.getMethod();

    const LocationConfig *location = getLocation(target, *config);
    if (location == nullptr)
    {
        Log::warning("No matching location found for target: " + target);
        handleError(404, response, config);
        return response;
    }

    response.setStatus(200);
    response.addHeader("Content-Type", "text/plain");
    response.setBody("Hello, World! You requested " + target + " on " + request.getHeaders().getHost().value_or("") +
                     ". Current resource is " + location->getPath());
    response.setComplete();
    return response;
}

void Router::handleError(int statusCode, HttpResponse &response, AConfig *config) const
{
    response = ErrorHandler::getErrorResponse(statusCode, config);
    response.setComplete();
}

const LocationConfig *Router::getLocation(const std::string &path, const ServerConfig &serverConfig) const
{
    const LocationConfig *bestMatch = nullptr;
    size_t maxSize = 0;

    std::vector<std::string> locations = serverConfig.getLocationPaths();

    for (const auto &locPath : locations)
    {
        if (!path.starts_with(locPath))
        {
            continue;
        }
        if (locPath.length() > maxSize &&
            (path.length() == locPath.length() || locPath.back() == '/' || path[locPath.length()] == '/'))
        {
            maxSize = locPath.length();
            bestMatch = serverConfig.getLocation(locPath);
            Log::debug("Found new best match: " + locPath);
        }
    }

    if (bestMatch == nullptr)
    {
        Log::warning("No location matched for path: " + path);
    }
    return bestMatch;
}