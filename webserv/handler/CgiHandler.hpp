#pragma once

#include <webserv/handler/AHandler.hpp> // for AHandler
#include <webserv/http/HttpRequest.hpp> // for HttpRequest
#include <webserv/socket/CgiSocket.hpp> // for CgiSocket

#include <memory> // for unique_ptr
#include <string> // for string
#include <vector> // for vector

#include <stddef.h> // for size_t
#include <stdint.h> // for uint8_t

class CgiProcess;
class HttpResponse;

class CgiHandler : public AHandler
{
  public:
    CgiHandler(const HttpRequest &request, HttpResponse &response);

    CgiHandler(const CgiHandler &other) = delete;
    CgiHandler &operator=(const CgiHandler &other) = delete;
    CgiHandler(CgiHandler &&other) noexcept = delete;
    CgiHandler &operator=(CgiHandler &&other) noexcept = delete;

    ~CgiHandler() = default;

    void handle() override;
    void wait() noexcept;
    void setCgiSockets(std::unique_ptr<CgiSocket> cgiStdIn, std::unique_ptr<CgiSocket> cgiStdOut,
                       std::unique_ptr<CgiSocket> cgiStdErr);
    void setPid(int pid);

  protected:
    void handleTimeout() override;

  private:
    constexpr static size_t bufferSize_ = 8192; // TODO: remove duplicate definition and move to configmanager
    std::vector<uint8_t> buffer_;

    std::unique_ptr<CgiProcess> cgiProcess_;
    std::unique_ptr<CgiSocket> cgiStdIn_;
    std::unique_ptr<CgiSocket> cgiStdOut_;
    std::unique_ptr<CgiSocket> cgiStdErr_;
    void parseCgiOutput();
    void parseCgiHeaders(std::string &headers);
    void finalizeCgiResponse();
    void appendToBuffer(const char *data, size_t length);

    int pid_ = -1;

    void write();
    void read();
    void error();
};