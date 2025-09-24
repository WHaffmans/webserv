#pragma once

#include <string> // for string

#include <stddef.h> // for size_t

std::string trimSemi(const std::string &str);
std::string trim(const std::string &str);
size_t findCorrespondingClosingBrace(const std::string &str, size_t openPos);