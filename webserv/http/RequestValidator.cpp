#include "webserv/log/Log.hpp"

#include <webserv/config/AConfig.hpp>
#include <webserv/http/RequestValidator.hpp>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

RequestValidator::RequestValidator(const AConfig *config, const HttpRequest *request) : config(config), request(request)
{
}

std::optional<RequestValidator::ValidationError> RequestValidator::validate() const
{
    if (auto error = validateContentLength())
    {
        return error;
    }
    if (auto error = validateMethod())
    {
        return error;
    }
    if (auto error = validateHttpVersion())
    {
        return error;
    }
    if (auto error = validateHostHeader())
    {
        return error;
    }
    return std::nullopt; // No validation errors
}

std::optional<RequestValidator::ValidationError> RequestValidator::validateHostHeader() const
{
    if (!request->getHeaders().has("Host"))
    {
        return ValidationError{400, "Bad Request: Missing Host header"};
    }
    std::string hostHeader = request->getHeaders().get("Host");
    // Basic validation: check if host header is not empty
    if (hostHeader.empty())
    {
        return ValidationError{400, "Bad Request: Empty Host header"};
    }
    Log::debug("Host header validated: " + hostHeader);
    return std::nullopt;
}

std::optional<RequestValidator::ValidationError> RequestValidator::validateHttpVersion() const
{
    std::string httpVersion = request->getHttpVersion();
    if (httpVersion != "HTTP/1.1" && httpVersion != "HTTP/1.0")
    {
        return ValidationError{505, "HTTP Version Not Supported"};
    }
    return std::nullopt;
}

std::optional<RequestValidator::ValidationError> RequestValidator::validateContentLength() const
{
    size_t bodySize = request->getBody().size();
    size_t maxBodySize = config->get<size_t>("client_max_body_size").value_or(1024 * 1024);
    if (bodySize > maxBodySize) // exceed server limit
    {
        return ValidationError{413, "Payload Too Large"};
    }
    // If Content-Length header is present, validate it
    if (request->getHeaders().getContentLength())
    {
        size_t contentLength = *request->getHeaders().getContentLength();
        if (contentLength != bodySize)
        {
            return ValidationError{400, "Bad Request: Content-Length does not match body size"};
        }
    }
    return std::nullopt; // Content-Length is valid
}

std::optional<RequestValidator::ValidationError> RequestValidator::validateMethod() const
{
    auto allowedMethodsOpt = config->get<std::vector<std::string>>("allowed_methods");
    std::vector<std::string> allowedMethods;

    if (request->getMethod().empty())
    {
        return ValidationError{.statusCode=400, .message="Bad Request: Empty or Invalid HTTP Method"};
    }

    std::vector<std::string> possibleMethods = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH"};
    if (std::ranges::find(possibleMethods, request->getMethod()) == possibleMethods.end())
    {
        return ValidationError{.statusCode=501, .message="Method Not Implemented"};
    }
    
    if (allowedMethodsOpt.has_value())
    {
        allowedMethods = allowedMethodsOpt.value();
    }
    else
    {
        allowedMethods = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS"};
    }
    for (const std::string &method : allowedMethods)
    {
        if (method == request->getMethod())
        {
            return std::nullopt; // Method is allowed
        }
    }

    return ValidationError{.statusCode=405, .message="Method Not Allowed"};
}

RequestValidator::ValidationException::ValidationException(int code) : code_(code) {};

int RequestValidator::ValidationException::code() const noexcept
{
    return code_;
}

const char *RequestValidator::ValidationException::what() const noexcept
{
    return "Request validation failed";
}