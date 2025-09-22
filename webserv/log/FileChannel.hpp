#pragma once

#include <webserv/log/Channel.hpp>

#include <fstream>
#include <map>
#include <string>

class FileChannel : public Channel
{
  public:
    FileChannel(const std::string &filename, std::ios_base::openmode mode, Log::Level logLevel = Log::Level::Trace);

    FileChannel(const FileChannel &other) = delete;
    FileChannel(FileChannel &&other) = delete;
    FileChannel &operator=(const FileChannel &other) = delete;
    FileChannel &operator=(FileChannel &&other) = delete;

    ~FileChannel() override;
    void log(const Log::Level &logLevel, const std::string &message,
             const std::map<std::string, std::string> &context = {}) override;

  private:
    std::string filename_;
    std::ofstream fileStream_;
};