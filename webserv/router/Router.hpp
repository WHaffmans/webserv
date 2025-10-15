#pragma once

#include <webserv/config/AConfig.hpp> // for AConfig
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
    Router(Client *client);
    void handleRequest();

  private:
    Client *client_;
    [[nodiscard]] bool isMethodSupported(const std::string &method, const AConfig &config);
};