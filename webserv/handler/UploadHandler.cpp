#include "webserv/http/HttpConstants.hpp"
#include "webserv/utils/FileUtils.hpp"

#include <webserv/config/AConfig.hpp>
#include <webserv/handler/ErrorHandler.hpp>
#include <webserv/handler/URI.hpp>
#include <webserv/handler/UploadHandler.hpp>
#include <webserv/http/HttpRequest.hpp>
#include <webserv/http/HttpResponse.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/utils/AutoIndex.hpp>
#include <webserv/utils/utils.hpp>

#include <algorithm>
#include <cstring>
#include <ctime>
#include <fstream>
#include <random>
#include <ranges>
#include <sstream>
#include <string>

#include <sys/stat.h>
#include <unistd.h>

const std::string UploadHandler::DEFAULT_UPLOAD_STORE = "data/uploads";

UploadHandler::UploadHandler(const HttpRequest &request, HttpResponse &response)
    : AHandler(request, response),
      uploadStore_(request.getUri().getConfig()->get<std::string>("upload_store").value_or(DEFAULT_UPLOAD_STORE))
{
    Log::trace(LOCATION);
}

void UploadHandler::handle()
{
    Log::trace(LOCATION);

    // Check Content-Type header
    auto contentType = request_.getHeaders().getContentType();
    if (!contentType.has_value())
    {
        Log::warning("Upload request missing Content-Type header");
        ErrorHandler::createErrorResponse(Http::StatusCode::BAD_REQUEST, response_);
        return;
    }

    const auto *locationConfig = dynamic_cast<const LocationConfig *>(request_.getUri().getConfig());
    if (locationConfig != nullptr && request_.getTarget() != locationConfig->getPath())
    {
        Log::warning("Upload request target does not match location path");
        ErrorHandler::createErrorResponse(Http::StatusCode::FORBIDDEN, response_);
        return;
    }

    if (contentType->find("multipart/form-data") == std::string::npos)
    {
        Log::debug("Upload request with non-multipart Content-Type: " + *contentType);
        response_.setStatus(200);
        response_.addHeader("Content-Type", "application/json");
        response_.setBody("{\"success\": true, \"message\": \"Form data received\"}\n");
        return;
    }

    if (!FileUtils::isDirectory(uploadStore_))
    {
        ErrorHandler::createErrorResponse(Http::StatusCode::FORBIDDEN, response_);
        return;
    }
    try
    {
        parseMultipart();

        response_.setStatus(Http::StatusCode::CREATED);
        // response_.addHeader("Content-Type", "application/json");
        if (request_.getUri().getQuery().find("autoindex") != std::string::npos)
        {
            auto redirectUrl = request_.getUri().getUriForPath(request_.getUri().getDir());
            response_.setBody(R"(<html><head><meta http-equiv="refresh" content="0; URL=')" + redirectUrl
                              + "'\" /></head><body></body></html>");
        }
        else
        {
            response_.setBody(R"({"success": true, "message": "Files uploaded successfully"}\n)");
        }
        //
    }
    catch (const std::exception &e)
    {
        Log::error("Error processing upload: " + std::string(e.what()));
        ErrorHandler::createErrorResponse(Http::StatusCode::BAD_REQUEST, response_);
    }
}

void UploadHandler::handleTimeout()
{
    Log::warning("Upload handler timeout");
    ErrorHandler::createErrorResponse(Http::StatusCode::GATEWAY_TIMEOUT, response_);
}

void UploadHandler::parseMultipart()
{
    Log::trace(LOCATION);

    auto contentType = request_.getHeaders().getContentType();
    if (!contentType.has_value())
    {
        throw std::runtime_error("Missing Content-Type header");
    }

    std::string boundary = extractBoundary(*contentType);
    std::string fullBoundary = "--" + boundary;

    const std::string &body = request_.getBody();
    // Find first boundary
    size_t pos = body.find(fullBoundary);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("No boundary found in body");
    }
    pos += fullBoundary.length();
    // Parse each part
    while (pos < body.length())
    {
        // Skip CRLF after boundary
        if (pos + 1 < body.length() && body[pos] == '\r' && body[pos + 1] == '\n')
        {
            pos += 2;
        }
        else if (body[pos] == '\n')
        {
            pos += 1;
        }

        // Find next boundary
        size_t nextBoundary = body.find(fullBoundary, pos);
        if (nextBoundary == std::string::npos)
        {
            break;
        }

        // Extract this part (excluding the boundary and preceding CRLF)
        size_t partEnd = nextBoundary;
        while (partEnd > pos && (body[partEnd - 1] == '\r' || body[partEnd - 1] == '\n'))
        {
            partEnd--;
        }

        std::string part = body.substr(pos, partEnd - pos);

        // Parse the part
        if (!part.empty())
        {
            decodeSection(part);
        }

        // Move to next part
        pos = nextBoundary + fullBoundary.length();

        // Check if this is the final boundary
        if (pos + 2 <= body.length() && body.substr(pos, 2) == "--")
        {
            break;
        }
    }
    Log::info("Parsed " + std::to_string(uploadedFiles_.size()) + " file(s) from multipart form data");
}

std::string UploadHandler::extractBoundary(const std::string &contentType)
{
    Log::trace(LOCATION);

    size_t boundaryPos = contentType.find("boundary=");
    if (boundaryPos == std::string::npos)
    {
        throw std::runtime_error("No boundary found in Content-Type");
    }
    std::string boundary = contentType.substr(boundaryPos + std::strlen("boundary=")); // "boundary=" is 9 chars
    boundary = utils::extractQuotedValue(boundary);
    if (boundary.empty())
    {
        throw std::runtime_error("Malformed boundary in Content-Type");
    }
    // Remove any trailing characters after semicolon or whitespace
    boundary = utils::trim(boundary, "\t\r\n ");
    Log::debug("Extracted boundary: " + boundary);
    return boundary;
}

bool UploadHandler::decodeSection(const std::string &part)
{
    Log::trace(LOCATION);

    size_t headerEnd = part.find(Http::Protocol::DOUBLE_CRLF);
    if (headerEnd == std::string::npos)
    {
        headerEnd = part.find("\n\n");
    }
    if (headerEnd == std::string::npos)
    {
        Log::warning("Malformed multipart part: no header/content separator");
        return false;
    }

    std::string headers = part.substr(0, headerEnd);
    size_t contentStart
        = headerEnd
          + (part[headerEnd] == '\r' ? 4 : 2); // TODO: DRY, we're also doing this in the http headers i believe
    if (contentStart >= part.length())
    {
        Log::debug("Empty multipart part");
        return false;
    }

    // Extract Content-Disposition header
    std::string disposition = getHeaderValue(headers, "Content-Disposition");
    if (disposition.empty())
    {
        Log::warning("Multipart part missing Content-Disposition header");
        return false;
    }

    std::string filename = getFileName(disposition);
    if (filename.empty())
    {
        Log::debug("Multipart part is a form field, not a file");
        return false;
    }

    std::string fieldName = getFieldName(disposition);

    // Extract Content-Type (optional for files)
    std::string fileContentType = getHeaderValue(headers, "Content-Type");
    if (fileContentType.empty())
    {
        fileContentType = "application/octet-stream";
    }
    // Extract file content
    std::vector<uint8_t> fileData(part.begin() + (long)contentStart, part.end());
    // Save the file
    UploadedFile info;
    info.fieldName = fieldName;
    info.filename = filename;
    info.contentType = fileContentType;
    info.size = fileData.size();
    if (!save(info, fileData))
    {
        throw std::runtime_error("Failed to save file: " + filename);
    }
    uploadedFiles_.push_back(info);
    Log::info("Successfully uploaded file: " + filename + " (" + std::to_string(info.size) + " bytes)");

    return true;
}

std::string UploadHandler::getHeaderValue(const std::string &headers, const std::string &key) const
{
    std::string search = key;
    std::ranges::transform(search.begin(), search.end(), search.begin(), ::tolower);

    std::istringstream stream(headers);
    std::string line;

    while (std::getline(stream, line))
    {
        // Remove trailing \r if present
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
        {
            continue;
        }

        std::string name = utils::trim(line.substr(0, colonPos));
        std::ranges::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name == search)
        {
            return utils::trim(line.substr(colonPos + 1));
        }
    }

    return "";
}

std::string UploadHandler::getFileName(const std::string &disposition)
{
    // Look for filename="..." or filename*=UTF-8''...
    size_t filenamePos = disposition.find("filename=");
    if (filenamePos == std::string::npos)
    {
        return "";
    }
    std::string filename = disposition.substr(filenamePos + std::strlen("filename="));
    filename = utils::extractQuotedValue(filename);
    if (filename.empty())
    {
        Log::warning("Malformed filename in Content-Disposition");
        return "";
    }
    // Unquoted - take until semicolon or end
    size_t endPos = filename.find(';');
    if (endPos != std::string::npos)
    {
        filename = filename.substr(0, endPos);
    }

    return utils::trim(filename);
}

std::string UploadHandler::getFieldName(const std::string &disposition) const
{
    size_t namePos = disposition.find("name=");
    if (namePos == std::string::npos)
    {
        return "";
    }
    std::string fieldName = disposition.substr(namePos + 5);
    // Handle quoted name
    if (!fieldName.empty() && fieldName[0] == '"')
    {
        fieldName = fieldName.substr(1);
        size_t endQuote = fieldName.find('"');
        if (endQuote != std::string::npos)
        {
            fieldName = fieldName.substr(0, endQuote);
        }
    }
    else
    {
        // Unquoted - take until semicolon or end
        size_t endPos = fieldName.find(';');
        if (endPos != std::string::npos)
        {
            fieldName = fieldName.substr(0, endPos);
        }
    }

    return utils::trim(fieldName);
}

std::string UploadHandler::sanitizeFilename(const std::string &filename) const
{
    std::string sanitized;
    sanitized.reserve(filename.length());
    for (char c : filename)
    {
        if (std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '.' || c == '-' || c == '_')
        {
            sanitized += c;
        }
        else if (c == ' ')
        {
            sanitized += '_';
        }
    }
    if (sanitized.empty() || sanitized == "." || sanitized == "..")
    {
        sanitized = "upload";
    }
    if (sanitized.length() > 255)
    {
        sanitized = sanitized.substr(0, 255);
    }
    return sanitized;
}

std::string UploadHandler::generateFilename(const std::string &baseFilename) const
{
    std::string sanitized = sanitizeFilename(baseFilename);
    std::string fullPath = uploadStore_ + "/" + sanitized;

    if (!FileUtils::isFile(fullPath))
    {
        return sanitized;
    }

    // File exists - add timestamp and random suffix
    std::string name = sanitized;
    std::string ext;

    size_t dotPos = sanitized.rfind('.');
    if (dotPos != std::string::npos)
    {
        name = sanitized.substr(0, dotPos);
        ext = sanitized.substr(dotPos);
    }

    // Generate unique suffix with timestamp + random number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1000, 9999);

    time_t now = time(nullptr);
    std::ostringstream oss;
    oss << name << "_" << now << "_" << dis(gen) << ext;

    return oss.str();
}

bool UploadHandler::save(UploadedFile &info, const std::vector<uint8_t> &data)
{
    Log::trace(LOCATION);

    std::string uniqueFilename = generateFilename(info.filename);
    std::string fullPath = FileUtils::joinPath(uploadStore_, uniqueFilename);
    Log::debug("Saving file to: " + fullPath);

    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open())
    {
        Log::error("Failed to open file for writing: " + fullPath);
        return false;
    }

    file.write(reinterpret_cast<const char *>(data.data()), static_cast<std::streamsize>(data.size()));
    file.close();
    if (!file.good())
    {
        Log::error("Error writing file: " + fullPath);
        return false;
    }

    info.savedPath = fullPath;
    return true;
}