#pragma once

#include <webserv/config/LocationConfig.hpp>
#include <webserv/handler/URIParser.hpp>
#include <webserv/http/HttpResponse.hpp> // for HttpResponse

#include <cstdint> // for uint8_t
#include <memory>  // for unique_ptr
#include <string>  // for string

class LocationConfig;
class URIParser;

class FileHandler
{
  public:
    FileHandler(const LocationConfig *location, const URIParser &uriParser);

    [[nodiscard]] std::unique_ptr<HttpResponse> getResponse() const;

  private:
    const LocationConfig *location_;
    const URIParser &uriParser_;

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