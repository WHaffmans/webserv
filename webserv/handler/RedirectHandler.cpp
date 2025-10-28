#include <webserv/handler/RedirectHandler.hpp> // for RedirectHandler
#include <webserv/http/HttpRequest.hpp>    // for HttpRequest
#include <webserv/http/HttpResponse.hpp>   // for HttpResponse
#include <webserv/log/Log.hpp>             // for Log, LOCATION
#include <webserv/handler/URI.hpp>         // for URI
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/http/HttpConstants.hpp>  // for HttpConstants


RedirectHandler::RedirectHandler(const HttpRequest &request, HttpResponse &response)
    : AHandler(request, response)
{
}

void RedirectHandler::handle()
{
    if (request_.getUri().isRedirect())
    {
        Log::info("Redirecting request to: " + request_.getUri().getRedirect().second + " with reason: "
                  + Http::getStatusCodeReason(request_.getUri().getRedirect().first));
        std::pair<int, std::string> redirect = request_.getUri().getRedirect();
        response_.setStatus(redirect.first);
        response_.addHeader(std::string(Http::Header::REDIRECT_LOCATION), redirect.second);
        response_.addHeader(std::string(Http::Header::CONTENT_TYPE), std::string(Http::MimeType::TEXT_HTML));
        response_.addHeader(std::string(Http::Header::CACHE_CONTROL), "no-cache");
        std::string body = "<html><head><title>" + std::to_string(redirect.first) + " " + Http::getStatusCodeReason(redirect.first) + "</title></head>"
                           "<body><a href=\"" +
                           redirect.second + "\">Found</a></body></html>";
        response_.setBody(body);
        
    }
}

void RedirectHandler::handleTimeout()
{

    Log::warning("Redirect handler timeout occurred for path: " + request_.getUri().getFullPath());
    ErrorHandler::createErrorResponse(504, response_, request_.getUri().getConfig());
}

