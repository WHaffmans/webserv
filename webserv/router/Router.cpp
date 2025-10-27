#include <webserv/router/Router.hpp>                   // for Router

#include <webserv/handler/ErrorHandler.hpp>
#include <webserv/http/RequestValidator.hpp>

#include <webserv/client/Client.hpp>                   // for Client
#include <webserv/config/AConfig.hpp>                  // for AConfig
#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValue
#include <webserv/handler/CgiHandler.hpp>              // for CgiHandler
#include <webserv/handler/CgiProcess.hpp>              // for CgiProcess
#include <webserv/handler/FileHandler.hpp>             // for FileHandler
#include <webserv/handler/URI.hpp>                     // for URI
#include <webserv/http/HttpRequest.hpp>                // for HttpRequest
#include <webserv/log/Log.hpp>                         // for Log, LOCATION

#include <exception> // for exception
#include <format>    // for vector
#include <memory>    // for unique_ptr, make_unique
#include <optional>  // for optional
#include <ranges>    // for __find_fn, find
#include <string>    // for basic_string, string, operator+
#include <vector>    // for vector

class HttpResponse;

Router::Router(Client *client) : client_(client)
{
    Log::trace(LOCATION);
}

// bool Router::isMethodSupported(const std::string &method, const AConfig &config) noexcept
// {
//     const ADirective *allowedMethods = config.getDirective("allowed_methods");
//     if (allowedMethods == nullptr || !allowedMethods->getValue().try_get<std::vector<std::string>>().has_value())
//     {
//         return true;
//     }
//     auto methods = allowedMethods->getValue().get<std::vector<std::string>>();
//     return std::ranges::find(methods, method) != methods.end();
// }

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
    const AConfig *config = request.getUri().getConfig();

    auto validator = std::make_unique<RequestValidator>(config, &request);
    auto error = validator->validate();
    if (error.has_value())
    {
        Log::warning("Request validation failed: " + error->message);
        throw RequestValidator::ValidationException{error->statusCode};
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
        }
    }
    else
    {
        return std::make_unique<FileHandler>(request, response);
    }
    return nullptr;
}