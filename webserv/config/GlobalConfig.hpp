#pragma once

#include <webserv/config/AConfig.hpp>      // for AConfig
#include <webserv/config/ServerConfig.hpp> // for ServerConfig

#include <memory> // for unique_ptr
#include <string> // for string
#include <vector> // for vector

class GlobalConfig : public AConfig
{
  public:
    GlobalConfig() = delete;
    GlobalConfig(const std::string &baseDir, const std::string &Block);

    GlobalConfig(const GlobalConfig &other) = delete;
    GlobalConfig &operator=(const GlobalConfig &other) = delete;
    GlobalConfig(GlobalConfig &&other) noexcept = delete;
    GlobalConfig &operator=(GlobalConfig &&other) noexcept = delete;

    ~GlobalConfig() override = default;

    // [[nodiscard]] std::string getName() const override;
    [[nodiscard]] std::string getType() const override;

    [[nodiscard]] std::vector<ServerConfig *> getServerConfigs() const;

  private:
    // AConfig *parent_;
    std::vector<std::unique_ptr<ServerConfig>> servers_;

    void parseBlock(const std::string &block) override;
};