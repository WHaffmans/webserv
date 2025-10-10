#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class CGISocket
{
  public:
    CGISocket(int readFd, int writeFd);

    CGISocket(const CGISocket &) = delete;
    CGISocket &operator=(const CGISocket &) = delete;
    CGISocket(CGISocket &&other) noexcept = default;
    CGISocket &operator=(CGISocket &&other) noexcept = default;

    ~CGISocket();

    // Get FDs for server epoll registration
    [[nodiscard]] int getReadFd() const;  // Server reads CGI output
    [[nodiscard]] int getWriteFd() const; // Server writes to CGI input

    // Handler interface
    ssize_t read(void *buffer, size_t size) const;
    ssize_t write(const void *buffer, size_t size) const;
    void setNonBlocking() const;

  private:
    int _readFd = -1;   // Server side of output pipe
    int _writeFd = -1;  // Server side of input pipe

};