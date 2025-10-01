#pragma once

#include <string>

namespace FileUtils
{
bool isDirectory(const std::string &path);
bool isFile(const std::string &path);
std::string getExtension(const std::string &filename);
std::string joinPath(const std::string &base, const std::string &addition);
} // namespace FileUtils