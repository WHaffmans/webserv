#pragma once

#include <cstdint>
#include <string>

class AutoIndex
{
  public:
    AutoIndex() = delete;
    AutoIndex(const AutoIndex &) = delete;
    AutoIndex &operator=(const AutoIndex &) = delete;
    AutoIndex(AutoIndex &&) = delete;
    AutoIndex &operator=(AutoIndex &&) = delete;
    ~AutoIndex() = delete;
    static std::string generate(const std::string &dir);

  private:
    static std::string formatFileSize(uintmax_t size);
};