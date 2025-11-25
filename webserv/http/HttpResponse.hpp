#pragma once

#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders
#include <webserv/log/Log.hpp>          // for LOCATION, Log

#include <cstdint> // for uint8_t, uint16_t
#include <memory>  // for unique_ptr
#include <string>  // for string
#include <vector>  // for vector

class Client;

class HttpResponse
{
  public:
    HttpResponse();

    HttpResponse(const HttpResponse &other) = delete;                 // Disable copy constructor
    HttpResponse &operator=(const HttpResponse &other) = delete;      // Disable copy assignment
    HttpResponse(HttpResponse &&other) noexcept = default;            // Move constructor
    HttpResponse &operator=(HttpResponse &&other) noexcept = default; // Move assignment

    ~HttpResponse() = default;

    void addHeader(const std::string &key, const std::string &value);

    void appendBody(const std::vector<uint8_t> &data);
    void appendBody(const std::string &body);

    void setBody(const std::vector<uint8_t> &data);
    void setBody(const std::string &body);

    void setComplete();
    void setError(uint16_t statusCode);

    void setStatus(uint16_t statusCode);

    [[nodiscard]] bool isComplete() const noexcept;

    [[nodiscard]] const HttpHeaders &getHeaders() const noexcept;

    [[nodiscard]] std::vector<uint8_t> toBytes(long offset = 0) const;

  private:
    [[nodiscard]] std::string getStatusLine() const;
    [[nodiscard]] std::string getContentLengthHeader() const;
    [[nodiscard]] static std::string getDateHeader();

    std::vector<uint8_t> body_;
    std::unique_ptr<HttpHeaders> headers_;
    bool complete_ = false;
    uint16_t statusCode_ = 200;
};