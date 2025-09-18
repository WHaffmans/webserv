#pragma once

#include <functional>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/server/Server.hpp>

class Server;

class Client
{
  public:
    Client(int client_fd, Server &server, const ServerConfig &server_config);
    void request(const std::string &req);
    std::string getResponse() const;


  private:
    int client_fd;
    std::reference_wrapper<Server> server;
    std::reference_wrapper<const ServerConfig> server_config;
};