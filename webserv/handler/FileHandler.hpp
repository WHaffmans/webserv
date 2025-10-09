#pragma once

#include "webserv/config/AConfig.hpp"

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
    FileHandler(const AConfig *config, const URI &uri);

    [[nodiscard]] std::unique_ptr<HttpResponse> getResponse() const;

  private:
    const AConfig *config_;
    const URI &uri_;

    enum ResourceType : uint8_t
    {
        FILE,
        DIRECTORY_INDEX,
        DIRECTORY_AUTOINDEX,
        NOT_FOUND
    };

    [[nodiscard]] ResourceType getResourceType(const std::string &path) const;
    [[nodiscard]] std::unique_ptr<HttpResponse> handleFile(const std::string &filepath) const;
    [[nodiscard]] std::unique_ptr<HttpResponse> handleDirectory(const std::string &dirpath, ResourceType type) const;
};