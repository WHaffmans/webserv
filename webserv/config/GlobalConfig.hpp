#pragma once

#include <webserv/config/AConfig.hpp> // for AConfig
#include <webserv/config/ServerConfig.hpp>

class GlobalConfig : public AConfig
{
  public:
    GlobalConfig() = delete;
    GlobalConfig(const std::string &Block);

    GlobalConfig(const GlobalConfig &other) = delete;
    GlobalConfig &operator=(const GlobalConfig &other) = delete;
    GlobalConfig(GlobalConfig &&other) noexcept = delete;
    GlobalConfig &operator=(GlobalConfig &&other) noexcept = delete;

    ~GlobalConfig() override = default;
    [[nodiscard]] std::vector<ServerConfig *> getServerConfigs() const;

  private:
    AConfig *parent_ = nullptr;
    std::vector<std::unique_ptr<ServerConfig>> servers_;

    void parseBlock(const std::string &block) override;
};