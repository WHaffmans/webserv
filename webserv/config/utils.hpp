#pragma once

#include <stddef.h>  // for size_t
#include <string>    // for string

std::string trimSemi(const std::string &str);
std::string trim(const std::string &str);
size_t findCorrespondingClosingBrace(const std::string &str, size_t openPos);