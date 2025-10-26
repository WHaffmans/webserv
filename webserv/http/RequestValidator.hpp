#pragma once

#include "webserv/config/AConfig.hpp"
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
    RequestValidator(const AConfig *config, const HttpRequest *request);

    [[nodiscard]] std::optional<ValidationError> validate() const;

  private:
    const AConfig *config;
    const HttpRequest *request;

    [[nodiscard]] std::optional<ValidationError> validateContentLength() const;
    [[nodiscard]] std::optional<ValidationError> validateMethod() const;
  };