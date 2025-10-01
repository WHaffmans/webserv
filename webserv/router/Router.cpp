#include "webserv/config/AConfig.hpp"
#include "webserv/config/ConfigManager.hpp"
#include "webserv/config/ServerConfig.hpp"
#include "webserv/handler/ErrorHandler.hpp"
#include "webserv/handler/FileHandler.hpp"
#include "webserv/handler/URIParser.hpp"
#include "webserv/http/HttpResponse.hpp"
#include "webserv/log/Log.hpp"

#include <webserv/router/Router.hpp>

#include <memory>
#include <string>

Router::Router() {}

std::unique_ptr<HttpResponse> Router::handleRequest(const HttpRequest &request) const
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

// void Router::handleError(int statusCode, HttpResponse &response, AConfig *config) const
// {
//     response = ErrorHandler::getErrorResponse(statusCode, config);
//     response.setComplete();
// }
