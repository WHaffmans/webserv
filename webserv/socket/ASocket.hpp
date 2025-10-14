#pragma once

#include <cstddef> // for size_t
#include <cstdint>

#include <sys/types.h> // for ssize_t

class ASocket
{
  public:
    enum class Type : uint8_t
    {
        CLIENT_SOCKET,
        SERVER_SOCKET,
        CGI_SOCKET
    };

    ASocket() = delete;
    explicit ASocket(int fd);

    ASocket(const ASocket &other) = delete;
    ASocket &operator=(const ASocket &other) = delete;
    ASocket(ASocket &&other) noexcept = default;
    ASocket &operator=(ASocket &&other) noexcept = default;

    virtual ~ASocket();

    [[nodiscard]] virtual Type getType() const = 0;
    [[nodiscard]] int getFd() const;

    ssize_t read(void *buf, size_t len) const;
    ssize_t write(const void *buf, size_t len) const;

  protected:
    void setNonBlocking() const;
    void setFd(int fd);

  private:
    int fd_;
};
