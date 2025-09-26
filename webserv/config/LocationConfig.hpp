#pragma once

#include <webserv/config/AConfig.hpp> // for AConfig

#include <string> // for string

class LocationConfig : public AConfig
{
  public:
    LocationConfig() = delete;
    LocationConfig(const std::string &Block, const AConfig *parent = nullptr);

    LocationConfig(const LocationConfig &other) = delete;
    LocationConfig &operator=(const LocationConfig &other) = delete;
    LocationConfig(LocationConfig &&other) noexcept = delete;
    LocationConfig &operator=(LocationConfig &&other) noexcept = delete;

    ~LocationConfig() override = default;

  private:
    void parseBlock(const std::string &block) override;
};