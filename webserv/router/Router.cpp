#include "webserv/handler/DeleteHandler.hpp"
#include <webserv/client/Client.hpp>                   // for Client
#include <webserv/config/AConfig.hpp>                  // for AConfig
#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValue
#include <webserv/handler/CgiHandler.hpp>              // for CgiHandler
#include <webserv/handler/CgiProcess.hpp>              // for CgiProcess
#include <webserv/handler/ErrorHandler.hpp>
#include <webserv/handler/FileHandler.hpp>     // for FileHandler
#include <webserv/handler/RedirectHandler.hpp> // for RedirectHandler
#include <webserv/handler/URI.hpp>             // for URI
#include <webserv/handler/UploadHandler.hpp>   // for UploadHandler
#include <webserv/http/HttpRequest.hpp>        // for HttpRequest
#include <webserv/http/RequestValidator.hpp>
#include <webserv/log/Log.hpp>       // for Log, LOCATION
#include <webserv/router/Router.hpp> // for Router

#include <exception> // for exception
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

std::unique_ptr<AHandler> Router::handleRequest()
{
    Log::trace(LOCATION);

    HttpRequest &request = client_->getHttpRequest();
    HttpResponse &response = client_->getHttpResponse();

    const AConfig *config = request.getUri().getConfig();

    auto validator = std::make_unique<RequestValidator>(config, &request);
    auto error = validator->validate();
    if (error.has_value())
    {
        Log::warning("Request validation failed: " + error->message);
        throw RequestValidator::ValidationException{error->statusCode};
    }
    if (request.getUri().isRedirect())
    {
        return std::make_unique<RedirectHandler>(request, response);
    }
    if (request.getMethod() == "DELETE" && !request.getUri().isCgi())
    {
        return std::make_unique<DeleteHandler>(request, response);
    }
    if (request.getUri().isUpload() && request.getMethod() == "POST")
    {
        Log::debug("Handling file upload");
        return std::make_unique<UploadHandler>(request, response);
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
            // Ensure we return an error response instead of a null handler to avoid hanging/connection drops
            ErrorHandler::createErrorResponse(500, response, config);
            return nullptr;
        }
    }
    else
    {
        return std::make_unique<FileHandler>(request, response);
    }
    return nullptr;
}