#pragma once

#include <webserv/http/HttpHeaders.hpp> // for HttpHeaders
#include <webserv/log/Log.hpp>          // for LOCATION, Log

#include <cstdint> // for uint8_t
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

    ~HttpResponse() { Log::trace(LOCATION); };

    void addHeader(const std::string &key, const std::string &value);

    void appendBody(const std::vector<uint8_t> &data);
    void appendBody(const std::string &body);

    void setBody(const std::vector<uint8_t> &data);
    void setBody(const std::string &body);

    void setComplete();

    void setStatus(int statusCode);

    [[nodiscard]] bool isComplete() const;

    [[nodiscard]] const HttpHeaders &getHeaders() const;

    [[nodiscard]] std::vector<uint8_t> toBytes() const;

  private:
    [[nodiscard]] std::string getStatusLine() const;
    [[nodiscard]] std::string getContentLength() const;

    std::vector<uint8_t> body_;
    std::unique_ptr<HttpHeaders> headers_;
    bool complete_ = false;
    int statusCode_ = 200;
};