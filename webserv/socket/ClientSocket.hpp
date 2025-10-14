#pragma once

#include <webserv/socket/ASocket.hpp> // for ASocket

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class ClientSocket : public ASocket
{
  public:
    explicit ClientSocket(int fd);

    [[nodiscard]] ASocket::Type getType() const override;
};