#pragma once

#include "webserv/config/LocationConfig.hpp"

#include <webserv/http/HttpRequest.hpp>
#include <webserv/http/HttpResponse.hpp>

#include <memory>
#include <string>

class Router
{
  public:
    Router();

    [[nodiscard]] std::unique_ptr<HttpResponse> handleRequest(const HttpRequest &request) const;

  private:
    // void handleError(int statusCode, HttpResponse &response, AConfig *config = nullptr) const;
    [[nodiscard]] const LocationConfig *getLocation(const std::string &path, const ServerConfig &serverConfig) const;
};