#include "webserv/handler/ErrorHandler.hpp"
#include "webserv/http/HttpResponse.hpp"

#include <webserv/config/LocationConfig.hpp>
#include <webserv/handler/FileHandler.hpp>
#include <webserv/handler/MIMETypes.hpp>
#include <webserv/handler/URIParser.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/utils/FileUtils.hpp>

#include <cerrno>  // for errno
#include <cstring> // for strerror, strlen
#include <fstream>
#include <memory>
#include <ranges> // for views
#include <string>
#include <vector>
// for FILE, fopen, fclose, fread, SEEK_END, SEEK
#include <sys/stat.h> // for stat, S_ISREG, S_ISDIR

FileHandler::FileHandler(const LocationConfig *location, const URIParser &uriParser)
    : location_(location), uriParser_(uriParser)
{
    Log::trace(LOCATION);
}

std::vector<char> FileHandler::readBinaryFile(const std::string &filepath)
{
    Log::trace(LOCATION);

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        Log::error("Failed to open file: " + filepath);
        return {};
    }

    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read entire file
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size))
    {
        Log::error("Failed to read file: " + filepath);
        return {};
    }

    return buffer;
}

std::string FileHandler::readFileAsString(const std::string &filepath)
{
    Log::trace(LOCATION);

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open())
    {
        return "";
    }

    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

std::unique_ptr<HttpResponse> FileHandler::getResponse() const
{
    Log::trace(LOCATION);
    auto response = std::make_unique<HttpResponse>();
    response->setStatus(200);
    std::string filepath = uriParser_.getFilePath();
    if (uriParser_.isFile())
    {
        std::string extension = uriParser_.getExtension();
        std::string mimeType = MIMETypes().getType(extension);
        response->addHeader("Content-Type", mimeType);

        std::vector<char> fileData = readBinaryFile(filepath);
        Log::debug("Serving file: " + filepath + " with MIME type: " + mimeType);
        if (fileData.empty())
        {
            return ErrorHandler::getErrorResponse(404, location_);
        }
        response->setBody(std::string(fileData.begin(), fileData.end()));
    }
    else if (uriParser_.isDirectory() && location_->get<std::vector<std::string>>("index").has_value())
    {
        auto possible_indexes = location_->get<std::vector<std::string>>("index").value();
        std::string indexPath;
        for (auto &index : possible_indexes)
        {
            indexPath = FileUtils::joinPath(filepath, index);
            Log::debug("Checking for index file: " + indexPath);
            if (FileUtils::isFile(indexPath))
            {
                Log::debug("Found index file: " + indexPath);
                break;
            }
            indexPath.clear();
        }

        std::vector<char> fileData = readBinaryFile(indexPath);
        Log::debug("Serving index file: " + indexPath);
        if (fileData.empty())
        {
            return ErrorHandler::getErrorResponse(404);
        }

        std::string extension = FileUtils::getExtension(indexPath);
        std::string mimeType = MIMETypes().getType(extension);
        response->addHeader("Content-Type", mimeType);
        response->setBody(std::string(fileData.begin(), fileData.end()));
    }

    else if (uriParser_.isDirectory() && location_->get<bool>("autoindex").value_or(false))
    {
        Log::debug("Requested path is a directory: " + filepath);
        return ErrorHandler::getErrorResponse(403);
    }
    else
    {
        return ErrorHandler::getErrorResponse(404);
    }
    return response;
}