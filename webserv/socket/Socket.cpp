#include <webserv/socket/Socket.hpp>

#include <webserv/log/Log.hpp>

#include <memory>
#include <stdexcept>

#include <arpa/inet.h>  // For inet_addr
#include <fcntl.h>      // For fcntl()"
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h>
#include <unistd.h> // For close()

Socket::Socket() : fd_(socket(AF_INET, SOCK_STREAM, 0))
{
    Log::trace(LOCATION);
    if (fd_ == -1)
    {
        Log::error("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }
    int opt = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        close(fd_);
        fd_ = -1;
        Log::error("setsockopt failed");
        throw std::runtime_error("setsockopt failed");
    }
    setNonBlocking();
}

Socket::Socket(int fd) : fd_(fd) // NOLINT(readability-identifier-naming)
{
    Log::trace(LOCATION);
    if (fd_ == -1)
    {
        Log::error("Invalid file descriptor");
        throw std::runtime_error("Invalid file descriptor");
    }
    setNonBlocking();
}

Socket::~Socket()
{
    Log::trace(LOCATION);
    if (fd_ != -1)
    {
        close(fd_);
    }
}

void Socket::listen(int backlog) const
{
    Log::trace(LOCATION);
    if (::listen(fd_, backlog) < 0)
    {
        Log::error("Listen failed");
        throw std::runtime_error("Listen failed");
    }
}

void Socket::bind(const std::string &host, const int port) const
{
    Log::trace(LOCATION);
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host.c_str());
    address.sin_port = htons(port);

    if (::bind(fd_, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0) //NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    {
        Log::fatal("Cannot bind to " + host + ":" + std::to_string(port) +
                   " - address already in use or permission denied");
        throw std::runtime_error("Bind failed");
    }
}

std::unique_ptr<Socket> Socket::accept() const
{
    Log::trace(LOCATION);
    int client_fd = ::accept(fd_, nullptr, nullptr);
    if (client_fd < 0)
    {
        Log::error("Accept failed");
        throw std::runtime_error("Accept failed");
    }
    return std::make_unique<Socket>(client_fd);
}

ssize_t Socket::recv(void *buf, size_t len) const
{
    Log::trace(LOCATION);
    return ::recv(fd_, buf, len, 0);
}

ssize_t Socket::send(const void *buf, size_t len) const
{
    Log::trace(LOCATION);
    return ::send(fd_, buf, len, 0);
}

void Socket::setNonBlocking() const
{
    Log::trace(LOCATION);
    if (fcntl(fd_, F_SETFL, O_NONBLOCK) < 0)
    {
        Log::error("Failed to set non-blocking mode");
        throw std::runtime_error("Failed to set non-blocking mode");
    }
}

int Socket::getFd() const
{
    Log::trace(LOCATION);
    return fd_;
}