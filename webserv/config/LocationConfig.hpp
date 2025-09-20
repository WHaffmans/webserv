#pragma once

#include <map>
#include <string>

class LocationConfig
{
  public:
    LocationConfig(const std::string &locationBlock);

  private:
    std::string path_;
    bool autoIndex_;
    std::string indexFile_;
    std::map<std::string, std::string> directives_;

    void parseLocationBlock(const std::string &block);
    void parseDirectives(const std::string &declarations);
};
