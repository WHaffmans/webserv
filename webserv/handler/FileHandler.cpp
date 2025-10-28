#include "webserv/utils/AutoIndex.hpp"

#include <webserv/config/AConfig.hpp>       // for AConfig
#include <webserv/handler/ErrorHandler.hpp> // for ErrorHandler
#include <webserv/handler/FileHandler.hpp>
#include <webserv/handler/MIMETypes.hpp>  // for MIMETypes
#include <webserv/handler/URI.hpp>        // for URI
#include <webserv/http/HttpConstants.hpp> // for NOT_FOUND, FORBIDDEN, OK
#include <webserv/http/HttpResponse.hpp>  // for HttpResponse
#include <webserv/log/Log.hpp>            // for Log, LOCATION
#include <webserv/utils/FileUtils.hpp>    // for joinPath, getExtension, isFile, readBinaryFile

#include <optional> // for optional
#include <ranges>   // for __find_if_fn, find_if
#include <string>   // for basic_string, allocator, operator+, string, char_traits
#include <vector>   // for vector

FileHandler::FileHandler(const HttpRequest &request, HttpResponse &response)
    : AHandler(request, response), uri_(request.getUri()), config_(uri_.getConfig())
{
    Log::trace(LOCATION);
}

void FileHandler::handleTimeout()
{
    Log::warning("File handler timeout occurred for path: " + uri_.getFullPath());
    ErrorHandler::createErrorResponse(504, response_, config_);
}

void FileHandler::handleFile(const std::string &filepath) const
{
    Log::trace(LOCATION);
    Log::debug("Requested path is a file: " + filepath);
    // auto response = std::make_unique<HttpResponse>();

    std::string extension = FileUtils::getExtension(filepath);
    std::string mimeType = MIMETypes().getType(extension);
    response_.addHeader("Content-Type", mimeType);

    std::vector<char> fileData = FileUtils::readBinaryFile(filepath);
    Log::debug("Serving file: " + filepath + " with MIME type: " + mimeType);
    if (fileData.empty())
    {
        ErrorHandler::createErrorResponse(Http::StatusCode::NOT_FOUND, response_, config_);
        return;
    }
    // TODO: annoying: For reading files, vector<char> is preferred, but for http data vector<uint8_t> is preferred
    response_.setBody(std::vector<uint8_t>{fileData.begin(), fileData.end()});
    response_.setStatus(Http::StatusCode::OK);
    // return response;
}

void FileHandler::handleDirectory(const std::string &dirpath, ResourceType type) const
{
    Log::trace(LOCATION);
    Log::debug("Requested path is a directory: " + dirpath);
    if (type == DIRECTORY_INDEX)
    {
        auto possible_indexes = config_->get<std::vector<std::string>>("index").value();
        auto first_matching = std::ranges::find_if(possible_indexes, [&](const std::string &index) {
            return FileUtils::isFile(FileUtils::joinPath(dirpath, index));
        });
        if (first_matching == possible_indexes.end())
        {
            ErrorHandler::createErrorResponse(Http::StatusCode::FORBIDDEN, response_, config_);
            return;
        }
        handleFile(FileUtils::joinPath(dirpath, *first_matching));
        return;
    }
    if (type == DIRECTORY_AUTOINDEX)
    {
        Log::debug("Requested path is a directory: " + dirpath);
        // ErrorHandler::createErrorResponse(Http::StatusCode::FORBIDDEN, response_, config_);
        // TODO: This doesn't trigger for some reason :p
        response_.setBody(AutoIndex::generate(dirpath));
        response_.setStatus(Http::StatusCode::OK);
        return;
    }
    ErrorHandler::createErrorResponse(Http::StatusCode::NOT_FOUND, response_, config_);
}

void FileHandler::handle()
{
    Log::trace(LOCATION);
    if (!uri_.isValid())
    {
        ErrorHandler::createErrorResponse(Http::StatusCode::NOT_FOUND, response_, config_);
        return;
    }
    std::string filepath = uri_.getFullPath();
    Log::debug("Handling file request for path: " + filepath);
    ResourceType resourceType = getResourceType(filepath);

    switch (resourceType)
    {
    case FILE: handleFile(filepath); return;
    case DIRECTORY_AUTOINDEX:
    case DIRECTORY_INDEX: handleDirectory(filepath, resourceType); return;
    case NOT_FOUND: ErrorHandler::createErrorResponse(Http::StatusCode::NOT_FOUND, response_, config_); return;
    }
    ErrorHandler::createErrorResponse(Http::StatusCode::NOT_FOUND, response_, config_);
}

FileHandler::ResourceType FileHandler::getResourceType(const std::string &path) const
{
    static_cast<void>(path);
    Log::trace(LOCATION);

    if (uri_.isFile())
    {
        return FILE;
    }
    if (uri_.isDirectory())
    {
        if (config_->get<std::vector<std::string>>("index").has_value())
        {
            return DIRECTORY_INDEX;
        }
        if (config_->get<bool>("autoindex").value_or(false))
        {
            return DIRECTORY_AUTOINDEX;
        }
    }
    return NOT_FOUND;
}