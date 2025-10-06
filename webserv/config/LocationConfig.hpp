#pragma once

#include <webserv/config/AConfig.hpp> // for AConfig

#include <string> // for string

class LocationConfig : public AConfig
{
  public:
    LocationConfig() = delete;
    LocationConfig(const std::string &Block, const std::string &path, const AConfig *parent = nullptr);

    LocationConfig(const LocationConfig &other) = delete;
    LocationConfig &operator=(const LocationConfig &other) = delete;
    LocationConfig(LocationConfig &&other) noexcept = delete;
    LocationConfig &operator=(LocationConfig &&other) noexcept = delete;

    ~LocationConfig() override = default;

    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] std::string getType() const override;
    [[nodiscard]] const std::string &getPath() const { return _path; }

  private:
    void parseBlock(const std::string &block) override;
    std::string _path;
};