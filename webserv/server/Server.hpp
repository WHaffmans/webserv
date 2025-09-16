#pragma once

#include "webserv/config/ConfigManager.hpp"

class Server
{
  public:
    Server(const ConfigManager &configManager);

    void start();
	void setupServerSocket(ServerConfig &config);
    void handleConnection(int epoll_fd, struct epoll_event *event);
    void handleRequest(int epoll_fd, struct epoll_event *event);

  private:
 
	int epoll_fd;
	std::string host;
	int port;

};
