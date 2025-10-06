#include <webserv/router/Router.hpp>

#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/ServerConfig.hpp>  // for ServerConfig
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/handler/FileHandler.hpp>  // for FileHandler
#include <webserv/handler/URIParser.hpp>    // for URIParser
#include <webserv/http/HttpHeaders.hpp>     // for HttpHeaders
#include <webserv/log/Log.hpp>              // for LOCATION, Log

#include <memory>   // for unique_ptr
#include <optional> // for optional
#include <string>   // for basic_string, string

class LocationConfig;

Router::Router() {}

std::unique_ptr<HttpResponse> Router::handleRequest(const HttpRequest &request)
{
    Log::trace(LOCATION);

    ServerConfig *config =
        ConfigManager::getInstance().getMatchingServerConfig(request.getHeaders().getHost().value_or(""));

    if (config == nullptr)
    {
        return ErrorHandler::getErrorResponse(400);
    }
    URIParser uriParser{request.getTarget(), *config};

    const std::string &target = request.getTarget();
    const std::string &method = request.getMethod();

    const LocationConfig *location = uriParser.getLocation();
    if (location == nullptr)
    {
        return ErrorHandler::getErrorResponse(404, config);
    }

    FileHandler fileHandler(location, uriParser);
    return fileHandler.getResponse();
}