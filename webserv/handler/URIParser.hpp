#pragma once

#include <webserv/config/LocationConfig.hpp>
#include <webserv/config/ServerConfig.hpp>
#include <webserv/server/Server.hpp>

#include <string> // for string, basic_string

class LocationConfig;
class ServerConfig;

class URIParser
{
  public:
    URIParser(const std::string &uri, const ServerConfig &serverConfig);

    [[nodiscard]] std::string getFilePath() const;
    [[nodiscard]] std::string getFilename() const;
    [[nodiscard]] std::string getExtension() const;

    [[nodiscard]] const LocationConfig *getLocation() const;

    [[nodiscard]] bool isFile() const;

    [[nodiscard]] bool isDirectory() const;
    [[nodiscard]] bool isValid() const;

  private:
    const LocationConfig *_locationConfig;
    std::string relativePath_;
    std::string root_;
};