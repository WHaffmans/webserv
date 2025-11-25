#pragma once

#include <webserv/handler/URI.hpp> // for URI

#include <cstdint> // for uintmax_t
#include <string>  // for string

class URI;

class AutoIndex
{
  public:
    AutoIndex() = delete;
    AutoIndex(const AutoIndex &) = delete;
    AutoIndex &operator=(const AutoIndex &) = delete;
    AutoIndex(AutoIndex &&) = delete;
    AutoIndex &operator=(AutoIndex &&) = delete;
    ~AutoIndex() = delete;
    static std::string generate(const std::string &dir, const URI &uri);

  private:
    static std::string formatFileSize(uintmax_t size);
};