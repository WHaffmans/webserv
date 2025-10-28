#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace FileUtils
{
bool isDirectory(const std::string &path);
bool isFile(const std::string &path);
bool isValidPath(const std::string &path);

std::string getExtension(const std::string &filename);
std::string joinPath(const std::string &base, const std::string &addition);

std::vector<char> readBinaryFile(const std::string &filepath);
std::string readFileAsString(const std::string &filepath);

std::vector<std::filesystem::directory_entry> listDirectory(const std::string &dirpath);
} // namespace FileUtils