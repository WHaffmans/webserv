#pragma once

#include <webserv/config/AConfig.hpp>        // for AConfig
#include <webserv/config/LocationConfig.hpp> // for LocationConfig

#include <map>    // for map
#include <memory> // for unique_ptr
#include <string> // for basic_string, string
#include <vector> // for vector

class ServerConfig : public AConfig
{
  public:
    ServerConfig() = delete;
    ServerConfig(const std::string &Block, const AConfig *parent = nullptr);

    ServerConfig(const ServerConfig &other) = delete;
    ServerConfig &operator=(const ServerConfig &other) = delete;
    ServerConfig(ServerConfig &&other) noexcept = delete;
    ServerConfig &operator=(ServerConfig &&other) noexcept = delete;

    ~ServerConfig() override = default;

    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] std::string getType() const override;

    [[nodiscard]] const LocationConfig *getLocation(const std::string &path) const;
    [[nodiscard]] std::vector<std::string> getLocationPaths() const;


  private:
    std::map<std::string, std::unique_ptr<LocationConfig>> locations_;
    AConfig *parent_ = nullptr;

    void parseBlock(const std::string &block) override;
};