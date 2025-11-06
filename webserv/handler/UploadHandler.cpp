#include <webserv/handler/UploadHandler.hpp>

#include <webserv/config/AConfig.hpp>
#include <webserv/handler/ErrorHandler.hpp>
#include <webserv/handler/URI.hpp>
#include <webserv/http/HttpRequest.hpp>
#include <webserv/http/HttpResponse.hpp>
#include <webserv/log/Log.hpp>
#include <webserv/utils/utils.hpp>

#include <algorithm>
#include <ctime>
#include <fstream>
#include <random>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

UploadHandler::UploadHandler(const HttpRequest &request, HttpResponse &response)
    : AHandler(request, response), maxFileSize_(0)
{
    Log::trace(LOCATION);

    // Get upload configuration from the matched location
    const AConfig *config = request_.getUri().getConfig();

    // Get upload directory from config (default: "uploads")
    uploadDir_ = config->get<std::string>("upload_store").value_or("data/uploads");

    // Get max file size from config (uses client_max_body_size)
    maxFileSize_ = config->get<size_t>("client_max_body_size").value_or(1048576); // Default 1MB

    Log::debug("Upload handler initialized", {
        {"upload_dir", uploadDir_},
        {"max_file_size", std::to_string(maxFileSize_)}
    });
}

void UploadHandler::handle()
{
    Log::trace(LOCATION);

    // Verify request method is POST
    if (request_.getMethod() != "POST")
    {
        Log::warning("Upload attempted with non-POST method: " + request_.getMethod());
        sendErrorResponse(405, "Method Not Allowed");
        return;
    }

    // Check Content-Type header
    auto contentType = request_.getHeaders().getContentType();
    if (!contentType.has_value())
    {
        Log::warning("Upload request missing Content-Type header");
        sendErrorResponse(400, "Missing Content-Type header");
        return;
    }

    // Check if it's multipart/form-data
    if (contentType->find("multipart/form-data") == std::string::npos)
    {
        // For application/x-www-form-urlencoded or other types, just return success
        // The upload endpoint can accept form data without files
        Log::debug("Upload request with non-multipart Content-Type: " + *contentType);
        response_.setStatus(200);
        response_.addHeader("Content-Type", "application/json");
        response_.setBody("{\"success\": true, \"message\": \"Form data received\"}\n");
        return;
    }

    // Check body size doesn't exceed limit
    if (request_.getBody().size() > maxFileSize_)
    {
        Log::warning("Upload request body exceeds max size: " + std::to_string(request_.getBody().size())
                    + " > " + std::to_string(maxFileSize_));
        sendErrorResponse(413, "Payload Too Large");
        return;
    }

    // Verify upload directory exists and is writable
    struct stat st = {};
    if (stat(uploadDir_.c_str(), &st) != 0)
    {
        Log::error("Upload directory does not exist: " + uploadDir_);
        sendErrorResponse(500, "Upload directory not configured");
        return;
    }
    if (!S_ISDIR(st.st_mode))
    {
        Log::error("Upload path is not a directory: " + uploadDir_);
        sendErrorResponse(500, "Upload directory not configured");
        return;
    }
    if (access(uploadDir_.c_str(), W_OK) != 0)
    {
        Log::error("Upload directory is not writable: " + uploadDir_);
        sendErrorResponse(500, "Upload directory not writable");
        return;
    }

    // Parse multipart form data
    try
    {
        parseMultipartFormData();

        // Success even if no files were uploaded (could be just form fields or empty file)
        sendSuccessResponse();
    }
    catch (const std::exception &e)
    {
        Log::error("Error processing upload: " + std::string(e.what()));
        sendErrorResponse(400, "Error processing upload");
    }
}

void UploadHandler::handleTimeout()
{
    Log::warning("Upload handler timeout");
    ErrorHandler::createErrorResponse(504, response_);
}

std::string UploadHandler::extractBoundary(const std::string &contentType) const
{
    Log::trace(LOCATION);

    size_t boundaryPos = contentType.find("boundary=");
    if (boundaryPos == std::string::npos)
    {
        throw std::runtime_error("No boundary found in Content-Type");
    }

    std::string boundary = contentType.substr(boundaryPos + 9); // "boundary=" is 9 chars

    // Remove quotes if present
    if (!boundary.empty() && boundary[0] == '"')
    {
        boundary = boundary.substr(1);
        size_t endQuote = boundary.find('"');
        if (endQuote != std::string::npos)
        {
            boundary = boundary.substr(0, endQuote);
        }
    }

    // Remove any trailing characters after semicolon or whitespace
    size_t endPos = boundary.find_first_of("; \t\r\n");
    if (endPos != std::string::npos)
    {
        boundary = boundary.substr(0, endPos);
    }

    Log::debug("Extracted boundary: " + boundary);
    return boundary;
}

void UploadHandler::parseMultipartFormData()
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
            parseMultipartPart(part);
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

bool UploadHandler::parseMultipartPart(const std::string &part)
{
    Log::trace(LOCATION);

    // Find the blank line separating headers from content
    size_t headerEnd = part.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
    {
        headerEnd = part.find("\n\n");
        if (headerEnd == std::string::npos)
        {
            Log::warning("Malformed multipart part: no header/content separator");
            return false;
        }
    }

    std::string headers = part.substr(0, headerEnd);
    size_t contentStart = headerEnd + (part[headerEnd] == '\r' ? 4 : 2);

    if (contentStart >= part.length())
    {
        Log::debug("Empty multipart part");
        return false;
    }

    // Extract Content-Disposition header
    std::string disposition = extractHeaderValue(headers, "Content-Disposition");
    if (disposition.empty())
    {
        Log::warning("Multipart part missing Content-Disposition header");
        return false;
    }

    // Extract filename - if no filename, this is a regular form field, not a file
    std::string filename = extractFilenameFromContentDisposition(disposition);
    if (filename.empty())
    {
        Log::debug("Multipart part is a form field, not a file");
        return false;
    }

    // Extract field name
    std::string fieldName = extractFieldNameFromContentDisposition(disposition);

    // Extract Content-Type (optional for files)
    std::string fileContentType = extractHeaderValue(headers, "Content-Type");
    if (fileContentType.empty())
    {
        fileContentType = "application/octet-stream";
    }

    // Extract file content
    std::vector<uint8_t> fileData(part.begin() + static_cast<std::string::difference_type>(contentStart), part.end());

    // Allow zero-length files
    if (fileData.size() > maxFileSize_)
    {
        Log::warning("File " + filename + " exceeds max size: " + std::to_string(fileData.size()));
        throw std::runtime_error("File too large: " + filename);
    }

    // Save the file
    UploadedFile info;
    info.fieldName = fieldName;
    info.filename = filename;
    info.contentType = fileContentType;
    info.size = fileData.size();

    if (!saveFile(filename, fileData, info))
    {
        throw std::runtime_error("Failed to save file: " + filename);
    }

    uploadedFiles_.push_back(info);
    Log::info("Successfully uploaded file: " + filename + " (" + std::to_string(info.size) + " bytes)");

    return true;
}

std::string UploadHandler::extractHeaderValue(const std::string &headers, const std::string &headerName) const
{
    std::string searchName = headerName;
    std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

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

        std::string name = line.substr(0, colonPos);
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        name = utils::trim(name);

        if (name == searchName)
        {
            std::string value = line.substr(colonPos + 1);
            return utils::trim(value);
        }
    }

    return "";
}

std::string UploadHandler::extractFilenameFromContentDisposition(const std::string &disposition) const
{
    // Look for filename="..." or filename*=UTF-8''...
    size_t filenamePos = disposition.find("filename=");
    if (filenamePos == std::string::npos)
    {
        return "";
    }

    std::string filename = disposition.substr(filenamePos + 9);

    // Handle quoted filename
    if (!filename.empty() && filename[0] == '"')
    {
        filename = filename.substr(1);
        size_t endQuote = filename.find('"');
        if (endQuote != std::string::npos)
        {
            filename = filename.substr(0, endQuote);
        }
    }
    else
    {
        // Unquoted - take until semicolon or end
        size_t endPos = filename.find(';');
        if (endPos != std::string::npos)
        {
            filename = filename.substr(0, endPos);
        }
    }

    return utils::trim(filename);
}

std::string UploadHandler::extractFieldNameFromContentDisposition(const std::string &disposition) const
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
        // Allow alphanumeric, dots, dashes, underscores
        if (std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '.' || c == '-' || c == '_')
        {
            sanitized += c;
        }
        else if (c == ' ')
        {
            sanitized += '_';
        }
        // Skip other characters
    }

    // Prevent directory traversal
    if (sanitized.empty() || sanitized == "." || sanitized == "..")
    {
        sanitized = "upload";
    }

    // Limit length
    if (sanitized.length() > 255)
    {
        sanitized = sanitized.substr(0, 255);
    }

    return sanitized;
}

std::string UploadHandler::generateUniqueFilename(const std::string &baseFilename) const
{
    std::string sanitized = sanitizeFilename(baseFilename);
    std::string fullPath = uploadDir_ + "/" + sanitized;

    // If file doesn't exist, use it as-is
    struct stat st = {};
    if (stat(fullPath.c_str(), &st) != 0)
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

bool UploadHandler::saveFile(const std::string &filename, const std::vector<uint8_t> &data, UploadedFile &info)
{
    Log::trace(LOCATION);

    std::string uniqueFilename = generateUniqueFilename(filename);
    std::string fullPath = uploadDir_ + "/" + uniqueFilename;

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

void UploadHandler::sendSuccessResponse()
{
    Log::trace(LOCATION);

    response_.setStatus(201); // Created

    // Build JSON response with uploaded file info
    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": true,\n";
    json << "  \"message\": \"Files uploaded successfully\",\n";
    json << "  \"files\": [\n";

    for (size_t i = 0; i < uploadedFiles_.size(); ++i)
    {
        const auto &file = uploadedFiles_[i];
        json << "    {\n";
        json << "      \"fieldName\": \"" << file.fieldName << "\",\n";
        json << "      \"filename\": \"" << file.filename << "\",\n";
        json << "      \"contentType\": \"" << file.contentType << "\",\n";
        json << "      \"size\": " << file.size << ",\n";
        json << "      \"path\": \"" << file.savedPath << "\"\n";
        json << "    }";

        if (i < uploadedFiles_.size() - 1)
        {
            json << ",";
        }
        json << "\n";
    }

    json << "  ]\n";
    json << "}\n";

    response_.addHeader("Content-Type", "application/json");
    response_.setBody(json.str());
}

void UploadHandler::sendErrorResponse(uint16_t statusCode, const std::string &message)
{
    Log::trace(LOCATION);

    response_.setStatus(statusCode);

    std::ostringstream json;
    json << "{\n";
    json << "  \"success\": false,\n";
    json << "  \"error\": \"" << message << "\"\n";
    json << "}\n";

    response_.addHeader("Content-Type", "application/json");
    response_.setBody(json.str());
}
