#pragma once

#include "webserv/config/AConfig.hpp"

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

    [[nodiscard]] bool isFile() const;
    [[nodiscard]] bool isDirectory() const;
    [[nodiscard]] bool isValid() const;

    [[nodiscard]] std::string getExtension() const;
    [[nodiscard]] const AConfig *getConfig() const;
    [[nodiscard]] const std::string &getBaseName() const;
    [[nodiscard]] const std::string &getFullPath() const;
    [[nodiscard]] const std::string &getDir() const;
    [[nodiscard]] const std::string &getPathInfo() const;
    [[nodiscard]] const std::string &getQuery() const;
    [[nodiscard]] const std::string &getFragment() const;

  private:
    void parseUri(const std::string &uri);
    void parseFullpath();

    std::string uriTrimmed_;
    const AConfig *config_;
    std::string fullPath_; // dir_ + baseName_ + pathInfo_
    std::string baseName_;
    std::string dir_;
    std::string pathInfo_;
    std::string query_;
    std::string fragment_;

    static const AConfig *matchConfig(const std::string &uri, const ServerConfig &serverConfig);
};