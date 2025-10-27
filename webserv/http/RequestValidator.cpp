#include <webserv/http/RequestValidator.hpp>

#include <webserv/config/AConfig.hpp>

#include <cstddef>
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
    return std::nullopt; // No validation errors
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

    if (allowedMethodsOpt.has_value())
    {
        allowedMethods = std::move(*allowedMethodsOpt);
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
    return ValidationError{405, "Method Not Allowed"};
}

RequestValidator::ValidationException::ValidationException(int code) : code_(code){};

int RequestValidator::ValidationException::code() const noexcept
{
    return code_;
}

const char *RequestValidator::ValidationException::what() const noexcept
{
    return "Request validation failed";
}