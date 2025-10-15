#pragma once

#include <webserv/config/ServerConfig.hpp>
#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders

#include <cstddef> // for size_t
#include <cstdint> // for uint8_t
#include <memory>
#include <string> // for string, basic_string

class Client;
class ServerConfig;
class URI;

class HttpRequest
{
  public:
    enum class State : std::uint8_t
    {
        RequestLine,
        Headers,
        Body,
        Chunked,
        Complete,
        ParseError
    };

    HttpRequest(Client *client);

    HttpRequest(const HttpRequest &other) = delete;
    HttpRequest(HttpRequest &&other) noexcept = delete;
    HttpRequest &operator=(const HttpRequest &other) = delete;
    HttpRequest &operator=(HttpRequest &&other) noexcept = delete;
    ~HttpRequest();

    [[nodiscard]] State getState() const;
    [[nodiscard]] const URI &getUri() const;
    [[nodiscard]] const HttpHeaders &getHeaders() const;
    [[nodiscard]] const std::string &getBody() const;
    [[nodiscard]] const std::string &getMethod() const;
    [[nodiscard]] const std::string &getTarget() const;
    [[nodiscard]] const std::string &getHttpVersion() const;
    [[nodiscard]] Client &getClient() const;

    void setState(State state);
    void receiveData(const char *data, size_t length);
    void reset();

  private:
    [[nodiscard]] bool parseBufferforRequestLine();
    [[nodiscard]] bool parseBufferforHeaders();
    [[nodiscard]] bool parseHeaderLine();
    [[nodiscard]] bool parseBufferforBody();
    [[nodiscard]] bool parseBufferforChunkedBody();

    void parseBuffer();
    void parseContentLength();

    Client *client_;

    State state_ = State::RequestLine;

    HttpHeaders headers_;

    std::unique_ptr<URI> uri_;

    std::string buffer_;
    std::string body_;
    std::string method_;
    std::string target_;
    std::string httpVersion_;
};
