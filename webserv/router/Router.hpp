#pragma once

#include <webserv/config/LocationConfig.hpp>
#include <webserv/http/HttpRequest.hpp>  // for HttpRequest
#include <webserv/http/HttpResponse.hpp> // for HttpResponse

#include <memory> // for unique_ptr
#include <string> // for string

class LocationConfig;
class ServerConfig;

class Router
{
  public:
    Router();

    [[nodiscard]] static std::unique_ptr<HttpResponse> handleRequest(const HttpRequest &request);

  private:
    [[nodiscard]] const LocationConfig *getLocation(const std::string &path, const ServerConfig &serverConfig) const;
    [[nodiscard]] static bool isMethodSupported(const std::string &method, const LocationConfig &location);
};