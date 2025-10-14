#pragma once

#include <cstddef>       // for size_t
#include <optional>      // for optional
#include <string>        // for basic_string, string, hash
#include <unordered_map> // for unordered_map

/**
 * @file HttpHeaders.hpp
 * @brief Declaration of the HttpHeaders class for managing HTTP headers.
 *
 * This class provides functionality to store, retrieve, and manage HTTP headers
 * in a structured manner.
 *
 * Without this class the HttpRequest and Response classes would become too bloated, and we'd end up adding members
 * to those classes for every new header we want to support.
 */
class HttpHeaders
{
  public:
    [[nodiscard]] const std::string &get(const std::string &name) const;
    [[nodiscard]] bool has(const std::string &name) const;

    void parse(const std::string &rawHeaders);
    void add(const std::string &name, const std::string &value);
    void remove(const std::string &name);

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::optional<size_t> getContentLength() const;
    [[nodiscard]] std::optional<std::string> getContentType() const;
    [[nodiscard]] std::optional<std::string> getHost() const;

  private:
    std::unordered_map<std::string, std::string> headers_;
};