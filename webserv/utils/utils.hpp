#pragma once

#include <webserv/socket/ASocket.hpp>

#include <cstddef> // for size_t
#include <cstdint>
#include <string> // for string
#include <vector>

namespace utils
{
size_t stoul(const std::string &str, int base = 10);
std::string trim(const std::string &str, const std::string &charset = " \t\n\r");
size_t findCorrespondingClosingBrace(const std::string &str, size_t openPos);
void removeEmptyLines(std::string &str);
void removeComments(std::string &str);

std::vector<std::string> split(const std::string &str, char delimiter);
std::string implode(const std::vector<std::string> &elements, const std::string &delimiter);

uint32_t stateToEpoll(const ASocket::IoState &event);
std::string uriEncode(const std::string &value);
std::string uriDecode(const std::string &value);
} // namespace utils