#pragma once

#include <webserv/config/AConfig.hpp>

#include <webserv/config/ServerConfig.hpp>
#include <webserv/http/HttpRequest.hpp>

#include <exception>
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

    class ValidationException : public std::exception
    {
      public:
        ValidationException(int code);
        [[nodiscard]] const char *what() const noexcept override;
        [[nodiscard]] int code() const noexcept;

      private:
        int code_;
    };

    RequestValidator(const AConfig *config, const HttpRequest *request);

    [[nodiscard]] std::optional<ValidationError> validate() const;

  private:
    const AConfig *config;
    const HttpRequest *request;

    [[nodiscard]] std::optional<ValidationError> validateContentLength() const;
    [[nodiscard]] std::optional<ValidationError> validateMethod() const;
};