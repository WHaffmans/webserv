#include <webserv/config/AConfig.hpp>                    // for AConfig
#include <webserv/config/directive/ADirective.hpp>       // for ADirective
#include <webserv/config/directive/DirectiveFactory.hpp> // for DirectiveFactory
#include <webserv/log/Log.hpp>                           // for Log, LOCATION
#include <webserv/utils/utils.hpp>                       // for trim

#include <sstream> // for basic_stringstream, stringstream
#include <utility> // for move, pair

AConfig::AConfig(const AConfig *parent) : parent_(parent) {}

void AConfig::addDirective(const std::string &line)
{
    auto directive = DirectiveFactory::createDirective(line);
    if (directive)
    {
        directives_.emplace_back(std::move(directive));
    }
}

const ADirective *AConfig::getDirective(const std::string &name) const
{
    for (const auto &directive : directives_)
    {
        if (directive->getName() == name)
        {
            return directive.get();
        }
    }
    if (parent_ != nullptr)
    {
        return parent_->getDirective(name);
    }
    return nullptr;
}

bool AConfig::hasDirective(const std::string &name) const
{
    for (const auto &directive : directives_)
    {
        if (directive->getName() == name)
        {
            return true;
        }
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
        Log::debug("Global Declaration: " + line);
        addDirective(line);
    }
}

std::string AConfig::getErrorPage(int statusCode) const
{
    // TODO
    const ADirective *directive = getDirective("error_page");
    for (const auto &directive : directives_)
    {
        if (directive->getName() == "error_page")
        {
            auto value = directive->getValueAs<std::pair<int, std::string>>();
            if (value.first == statusCode)
            {
                return value.second;
            }
        }
    }
    if (parent_ != nullptr)
    {
        return parent_->getErrorPage(statusCode);
    }
    return ""; // Return empty string if not found
}