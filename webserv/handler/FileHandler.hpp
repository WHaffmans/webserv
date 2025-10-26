#pragma once

#include <webserv/config/AConfig.hpp>
#include <webserv/config/LocationConfig.hpp>
#include <webserv/handler/AHandler.hpp> // for AHandler
#include <webserv/handler/URI.hpp>
#include <webserv/http/HttpRequest.hpp>  // for HttpRequest
#include <webserv/http/HttpResponse.hpp> // for HttpResponse

#include <cstdint> // for uint8_t
#include <memory>  // for unique_ptr
#include <string>  // for string

class AConfig;
class URI;
class HttpResponse;

class FileHandler : public AHandler
{
  public:
    FileHandler(const HttpRequest &request, HttpResponse &response);

    FileHandler(const FileHandler &other) = delete;
    FileHandler &operator=(const FileHandler &other) = delete;
    FileHandler(FileHandler &&other) noexcept = delete;
    FileHandler &operator=(FileHandler &&other) noexcept = delete;

    ~FileHandler() = default;

    void handle() override;

  protected:
    void handleTimeout() override;

  private:
    const URI &uri_;
    const AConfig *config_;

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