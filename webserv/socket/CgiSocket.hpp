#pragma once

#include <webserv/socket/ASocket.hpp> // for ASocket

#include <string> // for string, basic_string

#include <stddef.h> // for size_t
#include <sys/socket.h>
#include <sys/types.h> // for ssize_t
#include <unistd.h>

class CgiSocket : public ASocket
{
  public:
    explicit CgiSocket(int fd, ASocket::IoState event, std::string stream);

    [[nodiscard]] ASocket::Type getType() const noexcept override;

    ssize_t read(void *buf, size_t len) const override;
    ssize_t write(const void *buf, size_t len) const override;

    [[nodiscard]] std::string toString() const override;

  private:
    std::string stream_;
};