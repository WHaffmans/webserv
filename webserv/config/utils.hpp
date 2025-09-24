#pragma once

#include <cstddef> // for size_t
#include <string>  // for string

namespace utils
{
size_t stoul(const std::string &str, int base = 10);
std::string trimSemi(const std::string &str);
std::string trim(const std::string &str);
size_t findCorrespondingClosingBrace(const std::string &str, size_t openPos);
} // namespace utils
