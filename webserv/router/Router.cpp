#include <webserv/router/Router.hpp>

#include <webserv/config/directive/ADirective.hpp>

#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/ServerConfig.hpp>  // for ServerConfig
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/handler/FileHandler.hpp>  // for FileHandler
#include <webserv/handler/URIParser.hpp>    // for URIParser
#include <webserv/http/HttpHeaders.hpp>     // for HttpHeaders
#include <webserv/log/Log.hpp>              // for LOCATION, Log

#include <algorithm>
#include <memory>   // for unique_ptr
#include <optional> // for optional
#include <string>   // for basic_string, string
#include <vector>

class LocationConfig;

Router::Router() {}

bool Router::isMethodSupported(const std::string &method, const LocationConfig &location)
{
    const ADirective *allowedMethods = location.getDirective("allowed_methods");
    if (allowedMethods == nullptr || !allowedMethods->getValue().try_get<std::vector<std::string>>().has_value())
    {
        return true;
    }
    auto methods = allowedMethods->getValue().get<std::vector<std::string>>();
    return std::ranges::find(methods, method) != methods.end();
}

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
    static_cast<void>(target); // Suppress unused variable warning
    const std::string &method = request.getMethod();

    const LocationConfig *location = uriParser.getLocation();
    if (location == nullptr)
    {
        return ErrorHandler::getErrorResponse(404, config);
    }
    if (!isMethodSupported(method, *location))
    {
        return ErrorHandler::getErrorResponse(405, config);
    }
    FileHandler fileHandler(location, uriParser);
    return fileHandler.getResponse();
}