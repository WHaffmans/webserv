#pragma once

#include "webserv/config/ConfigManager.hpp"

class Server
{
  public:
	Server(const ConfigManager &configManager);

  private:
	std::string host;
	int port;
};
