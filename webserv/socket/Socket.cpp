#include <memory>
#include <webserv/socket/Socket.hpp>
#include <webserv/log/Log.hpp>

#include <stdexcept>
#include <unistd.h> // For close()

#include <arpa/inet.h>  // For inet_addr
#include <fcntl.h>      // For fcntl()"
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h>

Socket::Socket() : _fd(socket(AF_INET, SOCK_STREAM, 0))
{
    if (_fd == -1)
    {
        LOG_ERROR("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(_fd);
        _fd = -1;
        LOG_ERROR("setsockopt failed");
        throw std::runtime_error("setsockopt failed");
    }
    setNonBlocking();
}

Socket::Socket(int fd) : _fd(fd) // NOLINT(readability-identifier-naming)
{
    if (_fd == -1)
    {
        LOG_ERROR("Invalid file descriptor");   
        throw std::runtime_error("Invalid file descriptor");
    }
    setNonBlocking();
}

Socket::~Socket()
{
    if (_fd != -1)
    {
        close(_fd);
    }
}

void Socket::listen(int backlog) const
{
    if (::listen(_fd, backlog) < 0)
    {
        LOG_ERROR("Listen failed");
        throw std::runtime_error("Listen failed");
    }
}
void Socket::bind(const std::string &host, const int port) const
{

    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(port);

    if (::bind(_fd, (struct sockaddr *)&address, sizeof(address)) < 0) // NOLINT(cppcoreguidelines-pro-type-cstyle-cast
    {
        throw std::runtime_error("Bind failed");
    }
}

std::unique_ptr<Socket> Socket::accept() const
{
    int client_fd = ::accept(_fd, nullptr, nullptr);
    if (client_fd < 0)
    {
        LOG_ERROR("Accept failed");
        throw std::runtime_error("Accept failed");
    }
    return std::make_unique<Socket>(client_fd);
}

ssize_t Socket::recv(void *buf, size_t len) const
{
    return ::recv(_fd, buf, len, 0);
}

ssize_t Socket::send(const void *buf, size_t len) const
{
    return ::send(_fd, buf, len, 0);
}

void Socket::setNonBlocking() const
{
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        LOG_ERROR("Failed to set non-blocking mode");
        throw std::runtime_error("Failed to set non-blocking mode");
    }
}

int Socket::getFd() const
{
    return _fd;
}