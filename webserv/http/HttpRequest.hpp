#pragma once

#include "webserv/config/ServerConfig.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

class Client;
class HttpRequest
{
  public:
    enum class State : std::uint8_t
    {
        RequestLine,
        Headers,
        Body,
        Complete
    };

    HttpRequest(const ServerConfig *serverConfig, const Client *client);

    HttpRequest(const HttpRequest &other) = delete;
    HttpRequest(HttpRequest &&other) noexcept = delete;
    HttpRequest &operator=(const HttpRequest &other) = delete;
    HttpRequest &operator=(HttpRequest &&other) noexcept = delete;
    ~HttpRequest();

    [[nodiscard]] State getState() const;
    [[nodiscard]] const std::string &getHeaders() const;
    [[nodiscard]] const std::string &getBody() const;
    [[nodiscard]] size_t getContentLength() const;

    void receiveData(const char *data, size_t length);
    void reset();

  private:
    void parseBuffer();
    void parseContentLength();
    const ServerConfig *serverConfig_;
    const Client *client_;

    State state_ = State::RequestLine;

    std::string buffer_;

    std::string requestLine_;
    std::string headers_;
    std::string body_;
    size_t contentLength_ = 0;
};
