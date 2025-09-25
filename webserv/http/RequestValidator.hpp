#pragma once

#include <webserv/config/ServerConfig.hpp>
#include <webserv/http/HttpRequest.hpp>

#include <optional>
#include <string>

class RequestValidator
{
  public:
    struct ValidationError
    {
        int statusCode;
        std::string message;
    };

    RequestValidator() = delete;
    RequestValidator(const ServerConfig &config, const HttpRequest &request);
    RequestValidator(const RequestValidator &other) = delete;
    RequestValidator(RequestValidator &&other) = delete;

    RequestValidator &operator=(const RequestValidator &other) = delete;
    RequestValidator &operator=(RequestValidator &&other) = delete;

    ~RequestValidator() = default;

    [[nodiscard]] std::optional<ValidationError> validate() const;

  private:
    ServerConfig &config;
    HttpRequest &request;
};