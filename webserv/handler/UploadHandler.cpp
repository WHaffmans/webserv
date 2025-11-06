#include "webserv/handler/UploadHandler.hpp"

#include "webserv/handler/ErrorHandler.hpp"
#include "webserv/handler/URI.hpp"
#include "webserv/http/HttpConstants.hpp"

#include <webserv/log/Log.hpp> // for Log, LOCATION

#include <fstream>

UploadHandler::UploadHandler(const HttpRequest &request, HttpResponse &response) : AHandler(request, response) {}

void UploadHandler::handle()
{
    Log::trace(LOCATION);
    std::string fullPath = request_.getUri().getFullPath();
    if (fullPath.empty())
    {
        Log::warning("UploadHandler: Invalid path for UPLOAD");
        ErrorHandler::createErrorResponse(Http::StatusCode::BAD_REQUEST, response_, request_.getUri().getConfig());
        return;
    }
    uploadFile(fullPath, request_.getBody());
}

void UploadHandler::uploadFile(const std::string &path, const std::string &data)
{
    Log::trace(LOCATION);
    std::ofstream fileStream{path.c_str(), std::ios::binary};
    if (!fileStream)
    {
        Log::error("UploadHandler: Failed to open file for upload: " + path);
        ErrorHandler::createErrorResponse(Http::StatusCode::FORBIDDEN, response_, request_.getUri().getConfig());
        return;
    }
    fileStream.write(data.c_str(), data.size());
    fileStream.close();

    response_.setStatus(Http::StatusCode::CREATED);
    response_.setComplete();
}

void UploadHandler::handleTimeout()
{
    Log::warning("UploadHandler: Upload operation timed out");
    ErrorHandler::createErrorResponse(504, response_, request_.getUri().getConfig());
}