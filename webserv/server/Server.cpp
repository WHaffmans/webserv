#include <webserv/server/Server.hpp>
#include <iostream>

Server::Server(const ConfigManager &configManager)
{
	const auto &serverConfigs = configManager.getServerConfigs();
	if (serverConfigs.empty())
	{
		throw std::runtime_error("No server configurations available.");
	}
	// For simplicity, just take the first server config
	const ServerConfig &config = serverConfigs[0];
	host = config.getHost();
	port = config.getPort();
	std::cout << "Server initialized on " << host << ":" << port << '\n';
}