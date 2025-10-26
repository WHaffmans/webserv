#pragma once

#include <webserv/config/AConfig.hpp> // for AConfig
#include <webserv/config/LocationConfig.hpp>
#include <webserv/handler/AHandler.hpp>  // for AHandler
#include <webserv/http/HttpRequest.hpp>  // for HttpRequest
#include <webserv/http/HttpResponse.hpp> // for HttpResponse

#include <memory> // for unique_ptr
#include <string> // for string

class LocationConfig;
class ServerConfig;
class Client;

class Router
{
  public:
    Router(Client *client);
    [[nodiscard]] std::unique_ptr<AHandler> handleRequest();

  private:
    Client *client_;
    [[nodiscard]] bool isMethodSupported(const std::string &method, const AConfig &config) noexcept;
};