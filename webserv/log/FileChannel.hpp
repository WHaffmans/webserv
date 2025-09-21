#pragma once

#include <webserv/log/Channel.hpp>
#include <webserv/log/LogLevel.hpp>

#include <fstream>
#include <map>
#include <string>

class FileChannel : public Channel
{
  public:
    FileChannel(const std::string &filename, LogLevel logLevel = LogLevel::LOGLVL_TRACE);

    FileChannel(const FileChannel &other) = delete;
    FileChannel(const FileChannel &&other) = delete;
    FileChannel &operator=(const FileChannel &other) = delete;
    FileChannel &&operator=(const FileChannel &&other) = delete;

    ~FileChannel();
    void log(LogLevel &logLevel, const std::string &message,
             const std::map<std::string, std::string> &context = {}) override;

  private:
    std::string filename_;
    std::ofstream fileStream_;
};