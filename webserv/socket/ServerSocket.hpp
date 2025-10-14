#pragma once

#include "webserv/socket/ClientSocket.hpp"
#include <webserv/socket/ASocket.hpp>

#include <memory> // for unique_ptr
#include <string> // for string

class ServerSocket : public ASocket
{
  public:
    ServerSocket();

    ServerSocket(int fd);

    void listen(int backlog) const;
    void bind(const std::string &host, int port) const;

    [[nodiscard]] ASocket::Type getType() const override;

    [[nodiscard]] std::unique_ptr<ClientSocket> accept() const;
};