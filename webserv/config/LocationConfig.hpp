#pragma once

#include <map>
#include <string>

class LocationConfig
{
  public:
    LocationConfig(const std::string &locationBlock);

  private:
    std::string path;
    bool autoIndex;
    std::string indexFile;
    std::map<std::string, std::string> directives;

    void parseLocationBlock(const std::string &block);
    void parseDirectives(const std::string &declarations);
};
