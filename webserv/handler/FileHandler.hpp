#pragma once

#include "webserv/http/HttpRequest.hpp"

#include <webserv/config/AConfig.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/handler/URI.hpp>
#include <webserv/http/HttpResponse.hpp> // for HttpResponse

#include <cstdint> // for uint8_t
#include <memory>  // for unique_ptr
#include <string>  // for string

class AConfig;
class URI;

class FileHandler
{
  public:
    FileHandler(const HttpRequest &request, HttpResponse &response);

    FileHandler(const FileHandler &other) = delete;
    FileHandler(FileHandler &&other) noexcept = delete;
    FileHandler &operator=(const FileHandler &other) = delete;
    FileHandler &operator=(FileHandler &&other) noexcept = delete;

    ~FileHandler() = default;

    void handle() const;

  private:
    const AConfig *config_;
    const URI &uri_;

    HttpResponse &response_;

    enum ResourceType : uint8_t
    {
        FILE,
        DIRECTORY_INDEX,
        DIRECTORY_AUTOINDEX,
        NOT_FOUND
    };

    [[nodiscard]] ResourceType getResourceType(const std::string &path) const;
    void handleFile(const std::string &filepath) const;
    void handleDirectory(const std::string &dirpath, ResourceType type) const;
};