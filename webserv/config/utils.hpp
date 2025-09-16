#pragma once

#include <string>

std::string trimSemi(const std::string &str);
std::string trim(const std::string &str);
size_t findCorrespondingClosingBrace(const std::string &str, size_t openPos);