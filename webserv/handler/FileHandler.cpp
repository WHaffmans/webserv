#include <webserv/handler/FileHandler.hpp>

#include <webserv/config/LocationConfig.hpp> // for LocationConfig
#include <webserv/handler/ErrorHandler.hpp>  // for ErrorHandler
#include <webserv/handler/MIMETypes.hpp>  // for MIMETypes
#include <webserv/handler/URIParser.hpp>  // for URIParser
#include <webserv/http/HttpConstants.hpp> // for NOT_FOUND, FORBIDDEN, OK
#include <webserv/http/HttpResponse.hpp>  // for HttpResponse
#include <webserv/log/Log.hpp>            // for Log, LOCATION
#include <webserv/utils/FileUtils.hpp>    // for joinPath, getExtension, isFile, readBinaryFile

#include <functional> // for identity
#include <memory>     // for unique_ptr, allocator, make_unique
#include <optional>   // for optional
#include <ranges>     // for __find_if_fn, find_if
#include <string>     // for basic_string, string, operator+, char_traits
#include <utility>    // for move
#include <vector>     // for vector

FileHandler::FileHandler(const LocationConfig *location, const URIParser &uriParser)
    : location_(location), uriParser_(uriParser)
{
    Log::trace(LOCATION);
}

std::unique_ptr<HttpResponse> FileHandler::handleFile(const std::string &filepath) const
{
    Log::trace(LOCATION);
    auto response = std::make_unique<HttpResponse>();

    std::string extension = FileUtils::getExtension(filepath);
    std::string mimeType = MIMETypes().getType(extension);
    response->addHeader("Content-Type", mimeType);

    std::vector<char> fileData = FileUtils::readBinaryFile(filepath);
    Log::debug("Serving file: " + filepath + " with MIME type: " + mimeType);
    if (fileData.empty())
    {
        return ErrorHandler::getErrorResponse(Http::StatusCode::NOT_FOUND, location_);
    }
    // TODO: annoying: For reading files, vector<char> is preferred, but for http data vector<uint8_t> is preferred
    response->setBody(std::vector<uint8_t>{fileData.begin(), fileData.end()});
    response->setStatus(Http::StatusCode::OK);
    return response;
}

std::unique_ptr<HttpResponse> FileHandler::handleDirectory(const std::string &dirpath, ResourceType type) const
{
    Log::trace(LOCATION);

    if (type == DIRECTORY_INDEX)
    {
        auto possible_indexes = location_->get<std::vector<std::string>>("index").value();
        auto first_matching = std::ranges::find_if(possible_indexes, [&](const std::string &index) {
            return FileUtils::isFile(FileUtils::joinPath(dirpath, index));
        });
        if (first_matching == possible_indexes.end())
        {
            return ErrorHandler::getErrorResponse(Http::StatusCode::FORBIDDEN, location_);
        }
        return handleFile(FileUtils::joinPath(dirpath, *first_matching));
    }
    if (type == DIRECTORY_AUTOINDEX)
    {
        Log::debug("Requested path is a directory: " + dirpath);
        return ErrorHandler::getErrorResponse(Http::StatusCode::FORBIDDEN, location_);
    }
    return ErrorHandler::getErrorResponse(Http::StatusCode::NOT_FOUND, location_);
}

std::unique_ptr<HttpResponse> FileHandler::getResponse() const
{
    Log::trace(LOCATION);
    std::string filepath = uriParser_.getFilePath();
    ResourceType resourceType = getResourceType(filepath);

    switch (resourceType)
    {
    case FILE: return handleFile(filepath);
    case DIRECTORY_AUTOINDEX:
    case DIRECTORY_INDEX: return handleDirectory(filepath, resourceType);
    case NOT_FOUND: return ErrorHandler::getErrorResponse(Http::StatusCode::NOT_FOUND, location_);
    }
}

FileHandler::ResourceType FileHandler::getResourceType(const std::string &path) const
{
    Log::trace(LOCATION);

    if (uriParser_.isFile())
    {
        return FILE;
    }
    if (uriParser_.isDirectory())
    {
        if (location_->get<std::vector<std::string>>("index").has_value())
        {
            return DIRECTORY_INDEX;
        }
        if (location_->get<bool>("autoindex").value_or(false))
        {
            return DIRECTORY_AUTOINDEX;
        }
    }
    return NOT_FOUND;
}