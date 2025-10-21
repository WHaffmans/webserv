#include "webserv/handler/CgiProcess.hpp"

#include <webserv/config/AConfig.hpp>                  // for AConfig
#include <webserv/config/ConfigManager.hpp>            // for ConfigManager
#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValue
#include <webserv/handler/ErrorHandler.hpp>            // for ErrorHandler
#include <webserv/handler/FileHandler.hpp>             // for FileHandler
#include <webserv/handler/URI.hpp>                     // for URI
#include <webserv/http/HttpHeaders.hpp>                // for HttpHeaders
#include <webserv/log/Log.hpp>                         // for LOCATION, Log
#include <webserv/router/Router.hpp>

#include <memory>   // for unique_ptr
#include <optional> // for optional
#include <ranges>   // for __find_fn, find
#include <string>   // for basic_string, string
#include <vector>   // for vector

Router::Router(Client *client) : client_(client)
{
    Log::trace(LOCATION);
}

bool Router::isMethodSupported(const std::string &method, const AConfig &config) noexcept
{
    const ADirective *allowedMethods = config.getDirective("allowed_methods");
    if (allowedMethods == nullptr || !allowedMethods->getValue().try_get<std::vector<std::string>>().has_value())
    {
        return true;
    }
    auto methods = allowedMethods->getValue().get<std::vector<std::string>>();
    return std::ranges::find(methods, method) != methods.end();
}

std::unique_ptr<AHandler> Router::handleRequest()
{
    Log::trace(LOCATION);

    HttpRequest &request = client_->getHttpRequest();
    if (request.getState() == HttpRequest::State::ParseError)
    {
        Log::error("Router::handleRequest() called with incomplete request");
        
        return nullptr;
    }
    HttpResponse &response = client_->getHttpResponse();

    const std::string &target = request.getTarget();
    static_cast<void>(target); // Suppress unused variable warning
    const std::string &method = request.getMethod();

    const AConfig *config = request.getUri().getConfig();

    if (!isMethodSupported(method, *config))
    {
        return nullptr;
        // return ErrorHandler::getErrorResponse(405, config);
    }
    if (request.getUri().isCgi())
    {
        try
        {
            Log::debug("Starting CGI process");
            return std::make_unique<CgiHandler>(request, response);
        }
        catch (const std::exception &e)
        {
            Log::error("CGI process failed: " + std::string(e.what()));
            // return ErrorHandler::getErrorResponse(500, config);
        }
    }
    else
    {
        return std::make_unique<FileHandler>(request, response);
    }
    return nullptr;
}