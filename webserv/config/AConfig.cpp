#include <webserv/config/AConfig.hpp>                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>        // for ADirective
#include <webserv/config/directive/DirectiveFactory.hpp>  // for DirectiveFactory
#include <webserv/config/utils.hpp>                       // for trim
#include <webserv/log/Log.hpp>                            // for Log, LOCATION
#include <sstream>                                        // for basic_stringstream, stringstream
#include <utility>                                        // for move, pair

AConfig::AConfig(const AConfig *parent) : parent_(parent) {}

void AConfig::addDirective(const std::string &line)
{
    auto directive = DirectiveFactory::createDirective(line);
    if (directive)
    {
        directives_[directive->getName()] = std::move(directive);
    }
}

const ADirective *AConfig::getDirective(const std::string &name) const
{
    auto it = directives_.find(name);
    if (it != directives_.end())
    {
        return it->second.get();
    }
    if (parent_ != nullptr)
    {
        return parent_->getDirective(name);
    }
    return nullptr;
}

bool AConfig::hasDirective(const std::string &name) const
{
    if (directives_.contains(name)) // NOLINT
    {
        return true;
    }
    if (parent_ != nullptr)
    {
        return parent_->hasDirective(name);
    }
    return false;
}

void AConfig::parseDirectives(const std::string &declarations)
{
    Log::trace(LOCATION);
    std::stringstream ss(declarations);
    std::string line;
    while (ss.good())
    {
        std::getline(ss, line);
        line = utils::trim(line);
        if (line.empty())
        {
            continue;
        }
        Log::info("Global Declaration: " + line);
        auto directive = DirectiveFactory::createDirective(line);
        directives_[directive->getName()] = std::move(directive);
    }
}
