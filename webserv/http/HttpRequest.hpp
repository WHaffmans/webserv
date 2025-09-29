#pragma once

#include <webserv/config/ServerConfig.hpp>
#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders

#include <cstddef> // for size_t
#include <cstdint> // for uint8_t
#include <string>  // for string, basic_string

class Client;
class ServerConfig;

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
    void setState(State state);
    [[nodiscard]] const HttpHeaders &getHeaders() const;
    [[nodiscard]] const std::string &getBody() const;

    [[nodiscard]] const std::string &getMethod() const { return method_; }

    [[nodiscard]] const std::string &getTarget() const { return target_; }

    [[nodiscard]] const std::string &getHttpVersion() const { return httpVersion_; }

    void receiveData(const char *data, size_t length);
    void reset();

  private:
    void parseBuffer();
    [[nodiscard]] bool parseBufferforRequestLine();
    [[nodiscard]] bool parseBufferforHeaders();
    [[nodiscard]] bool parseHeaderLine();
    [[nodiscard]] bool parseBufferforBody();
    [[nodiscard]] bool parseBufferforChunkedBody();

    void parseContentLength();

    Client *client_;

    State state_ = State::RequestLine;

    HttpHeaders headers_;

    std::string buffer_;
    std::string body_;
    std::string method_;
    std::string target_;
    std::string httpVersion_;
    // std::string requestLine_;
    // std::string headers_;
    // size_t contentLength_ = 0;
};
