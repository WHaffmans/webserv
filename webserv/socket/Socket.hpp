#pragma once

#include <sys/types.h>
#include <string>

class Socket
{
  public:
    Socket();
    Socket(int fd); // NOLINT readability-identifier-naming

    Socket(const Socket &other) = delete;            // Disable copy constructor
    Socket &operator=(const Socket &other) = delete; // Disable copy assignment
    Socket(Socket &&other) noexcept;                 // Move constructor
    Socket &operator=(Socket &&other) noexcept;      // Move assignment

    ~Socket();

    void listen(int backlog) const;
    void bind(const std::string &host, const int port) const;
    [[nodiscard]] Socket accept() const;
    ssize_t recv(void *buf, size_t len) const;
    ssize_t send(const void *buf, size_t len) const;
    void setNonBlocking() const;
	[[nodiscard]] int getFd() const;

  private:
    int _fd;
};