#pragma once

#include <webserv/log/Channel.hpp> // for Channel
#include <webserv/log/Log.hpp>     // for Log

#include <fstream> // for basic_ofstream, ios_base, ofstream
#include <map>     // for map
#include <string>  // for string, basic_string

class FileChannel : public Channel
{
  public:
    FileChannel(const std::string &filename, std::ios_base::openmode mode);

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