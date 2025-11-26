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
    std::vector<UploadedFile> uploadedFiles_;

    void parse();

    bool decode(const std::string &part);
    bool save(UploadedFile &info, const std::vector<uint8_t> &data);

    [[nodiscard]] std::string generateFilename(const std::string &baseFilename) const;
    [[nodiscard]] static std::string extractBoundary(const std::string &contentType);

    static std::string getHeaderValue(const std::string &headers, const std::string &key);
    static std::string getFileName(const std::string &disposition);
    static std::string getFieldName(const std::string &disposition);
    static std::string sanitize(const std::string &filename);
};
