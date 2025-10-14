#include <webserv/config/AConfig.hpp>                    // for AConfig

#include <webserv/config/directive/ADirective.hpp>       // for ADirective
#include <webserv/config/directive/DirectiveFactory.hpp> // for DirectiveFactory
#include <webserv/config/directive/DirectiveValue.hpp>   // for DirectiveValue
#include <webserv/log/Log.hpp>                           // for Log, LOCATION
#include <webserv/utils/utils.hpp>                       // for trim

#include <sstream> // for basic_stringstream, stringstream
#include <utility> // for pair, move
#include <ranges>  // for filter

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

std::vector<const ADirective *> AConfig::getDirectives() const
{
    std::vector<const ADirective *> result;
    result.reserve(directives_.size());
    for (const auto &directive : directives_)
    {
        result.push_back(directive.get());
    }
    return result;
}

bool AConfig::has(const std::string &name) const
{
    if (owns(name))
    {
        return true;
    }

    if (parent_ != nullptr)
    {
        return parent_->has(name);
    }
    return false;
}

bool AConfig::owns(const std::string &name) const
{
    for (const auto &directive : directives_)
    {
        if (directive->getName() == name)
        {
            return true;
        }
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
    Log::trace(LOCATION);
    for (const auto &directive : directives_)
    {
        if (directive->getName() == "error_page")
        {
            auto value = directive->getValue().try_get<std::pair<int, std::string>>();
            if (value && value->first == statusCode)
            {
                return value->second;
            }
        }
    }
    if (parent_ != nullptr)
    {
        return parent_->getErrorPage(statusCode);
    }
    return ""; // Return empty string if not found
}

std::string AConfig::getCGIPath(const std::string &extension) const
{
    Log::trace(LOCATION);
    for (const auto &directive : directives_ | std::views::filter([](const auto &d) {
                                     return d->getName() == "cgi_ext";
                                 }))
    {
        
        if (!directive->getValue().holds<std::vector<std::string>>())
        {
            continue;
        }
        auto exts = directive->getValue().try_get<std::vector<std::string>>().value();
        {
            continue;
        }
        auto cgiPath = exts.back();
        exts.pop_back(); // Last element is the CGI path
        auto it = std::ranges::find(exts, extension);
        if (it != exts.end())
        {
            return cgiPath;
        }
    }
    if (parent_ != nullptr)
    {
        return parent_->getCGIPath(extension);
    }
    return {}; // Return empty string if not found
}