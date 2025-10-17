#pragma once

#include "webserv/handler/AHandler.hpp"
#include "webserv/socket/CgiSocket.hpp"

#include <memory>

class CgiProcess;

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
    void setCgiSockets(std::unique_ptr<CgiSocket> cgiStdIn, std::unique_ptr<CgiSocket> cgiStdOut);
    void setPid(int pid);

  private:
    constexpr static size_t bufferSize_ = 8192; // TODO: remove duplicate definition and move to configmanager
    std::unique_ptr<CgiProcess> cgiProcess_;
    std::unique_ptr<CgiSocket> cgiStdIn_;
    std::unique_ptr<CgiSocket> cgiStdOut_;
    int pid_ = -1;


    void write();
    void read();
};