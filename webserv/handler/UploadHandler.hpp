#pragma once

#include <webserv/handler/AHandler.hpp>

#include <cstdint>
#include <string>
#include <vector>

class HttpRequest;
class HttpResponse;

class UploadHandler : public AHandler
{
  public:
    UploadHandler(const HttpRequest &request, HttpResponse &response);

    UploadHandler(const UploadHandler &other) = delete;
    UploadHandler &operator=(const UploadHandler &other) = delete;
    UploadHandler(UploadHandler &&other) noexcept = delete;
    UploadHandler &operator=(UploadHandler &&other) noexcept = delete;

    ~UploadHandler() = default;

    void handle() override;

  protected:
    void handleTimeout() override;

  private:
    struct UploadedFile
    {
        std::string fieldName;
        std::string filename;
        std::string contentType;
        std::string savedPath;
        size_t size;
    };

    std::string uploadStore_;
    // size_t maxFileSize_;
    std::vector<UploadedFile> uploadedFiles_;

    void parseMultipart();
    bool save(UploadedFile &info, const std::vector<uint8_t> &data);
    std::string sanitizeFilename(const std::string &filename) const;
    std::string generateFilename(const std::string &baseFilename) const;

    // Multipart parsing helpers
    [[nodiscard]] static std::string extractBoundary(const std::string &contentType);
    bool decodeSection(const std::string &part);
    [[nodiscard]] std::string getHeaderValue(const std::string &headers, const std::string &key) const;
    [[nodiscard]] static std::string getFileName(const std::string &disposition);
    std::string getFieldName(const std::string &disposition) const;

    static const std::string DEFAULT_UPLOAD_STORE;
};
