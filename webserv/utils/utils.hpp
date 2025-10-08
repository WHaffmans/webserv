#pragma once

#include <cstddef> // for size_t
#include <string>  // for string
#include <vector>

namespace utils
{
size_t stoul(const std::string &str, int base = 10);
std::string trimSemi(const std::string &str);
std::string trim(const std::string &str);
size_t findCorrespondingClosingBrace(const std::string &str, size_t openPos);
void removeEmptyLines(std::string &str);
void removeComments(std::string &str);

std::vector<std::string> split(const std::string &str, char delimiter);
std::string implode(const std::vector<std::string> &elements, const std::string &delimiter);
} // namespace utils