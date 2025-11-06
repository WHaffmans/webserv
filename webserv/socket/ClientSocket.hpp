#pragma once

#include <webserv/socket/ASocket.hpp> // for ASocket

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class ClientSocket : public ASocket
{
  public:
    explicit ClientSocket(int fd, struct sockaddr address);

    [[nodiscard]] ASocket::Type getType() const noexcept override;
    [[nodiscard]] const struct sockaddr *getAddress() const noexcept;

    [[nodiscard]] std::string toString() const override;
  private:
    struct sockaddr address_;
};