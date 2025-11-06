#pragma once

#include <cstddef> // for size_t
#include <cstdint>
#include <functional> // for function
#include <string>

#include <sys/types.h> // for ssize_t

class ASocket
{
  public:
    enum class Type : uint8_t
    {
        CLIENT_SOCKET,
        SERVER_SOCKET,
        CGI_SOCKET,
        TIMER_SOCKET
    };

    enum class IoState : uint32_t
    {
        NONE = 0,
        READ = 1 << 0,
        WRITE = 1 << 1,
    };

    ASocket() = delete;
    explicit ASocket(int fd, IoState state = IoState::NONE);

    ASocket(const ASocket &other) = delete;
    ASocket &operator=(const ASocket &other) = delete;
    ASocket(ASocket &&other) noexcept = default;
    ASocket &operator=(ASocket &&other) noexcept = default;

    virtual ~ASocket();

    [[nodiscard]] virtual Type getType() const noexcept = 0;
    [[nodiscard]] int getFd() const noexcept;
    [[nodiscard]] IoState getEvent() const noexcept;
    [[nodiscard]] bool isDirty() const noexcept;

    void callback() const;
    void setCallback(std::function<void()> callback);

    virtual ssize_t read(void *buf, size_t len) const;
    virtual ssize_t write(const void *buf, size_t len) const;

    void setIOState(IoState event);
    void processed();

    [[nodiscard]] virtual std::string toString() const;

  protected:
    void setNonBlocking() const;
    void setFd(int fd);

  private:
    int fd_;
    bool dirty_ = false;
    IoState ioState_;
    std::function<void()> callback_ = nullptr;
};
