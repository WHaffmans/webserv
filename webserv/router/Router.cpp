#include "webserv/config/AConfig.hpp"

#include <webserv/config/ConfigManager.hpp> // for ConfigManager
#include <webserv/config/ServerConfig.hpp>  // for ServerConfig
#include <webserv/config/directive/ADirective.hpp>
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/handler/FileHandler.hpp>  // for FileHandler
#include <webserv/handler/URI.hpp>          // for URI
#include <webserv/http/HttpHeaders.hpp>     // for HttpHeaders
#include <webserv/log/Log.hpp>              // for LOCATION, Log
#include <webserv/router/Router.hpp>

#include <algorithm>
#include <memory>   // for unique_ptr
#include <optional> // for optional
#include <string>   // for basic_string, string
#include <vector>

class LocationConfig;

bool Router::isMethodSupported(const std::string &method, const AConfig &config)
{
    const ADirective *allowedMethods = config.getDirective("allowed_methods");
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

    ServerConfig *serverConfig =
        ConfigManager::getInstance().getMatchingServerConfig(request.getHeaders().getHost().value_or(""));

    if (serverConfig == nullptr)
    {
        return ErrorHandler::getErrorResponse(400);
    }
    URI uri{request, *serverConfig};

    const std::string &target = request.getTarget();
    static_cast<void>(target); // Suppress unused variable warning
    const std::string &method = request.getMethod();

    const AConfig *config = uri.getConfig();
    if (!isMethodSupported(method, *config))
    {
        return ErrorHandler::getErrorResponse(405, config);
    }
    FileHandler fileHandler(config, uri);
    return fileHandler.getResponse();
}