#pragma once

#include <memory>
#include <string>

#include <sys/types.h>

class Socket
{
  public:
    Socket();
    Socket(int fd); // NOLINT readability-identifier-naming

    Socket(const Socket &other) = delete;                 // Disable copy constructor
    Socket &operator=(const Socket &other) = delete;      // Disable copy assignment
    Socket(Socket &&other) noexcept = default;            // Move constructor
    Socket &operator=(Socket &&other) noexcept = default; // Move assignment

    ~Socket();

    void listen(int backlog) const;
    void bind(const std::string &host, const int port) const;
    [[nodiscard]] std::unique_ptr<Socket> accept() const;
    ssize_t recv(void *buf, size_t len) const;
    ssize_t send(const void *buf, size_t len) const;
    void setNonBlocking() const;
    [[nodiscard]] int getFd() const;

  private:
    int fd_;
};