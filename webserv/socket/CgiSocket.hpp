#pragma once

#include <webserv/socket/ASocket.hpp> // for ASocket

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class CgiSocket : public ASocket
{
  public:
    explicit CgiSocket(int fd);

    [[nodiscard]] ASocket::Type getType() const override;
};