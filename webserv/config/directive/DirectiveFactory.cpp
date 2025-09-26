#include <webserv/config/directive/BoolDirective.hpp>       // for BoolDirective
#include <webserv/config/directive/DirectiveFactory.hpp>   // for DirectiveFactory
#include <webserv/config/directive/IntDirective.hpp>        // for IntDirective
#include <webserv/config/directive/IntStringDirective.hpp>  // for IntStringDirective
#include <webserv/config/directive/SizeDirective.hpp>       // for SizeDirective
#include <webserv/config/directive/StringDirective.hpp>     // for StringDirective
#include <webserv/config/directive/VectorDirective.hpp>     // for VectorDirective
#include <webserv/config/utils.hpp>                         // for trim, trimSemi
#include <sstream>                                          // for basic_stringstream, stringstream
#include <stdexcept>                                        // for invalid_argument
#include <utility>                                          // for pair

#include "webserv/log/Log.hpp"                              // for LOCATION, Log

class ADirective;

std::unique_ptr<ADirective> DirectiveFactory::createDirective(const std::string &line)
{
    Log::trace(LOCATION);
    std::stringstream ss(line);
    std::string name;
    ss >> name;
    std::string arg;
    std::getline(ss, arg);

    std::string_view type;
    for (const auto &directive : supportedDirectives)
    {
        if (directive.name == name)
        {
            type = directive.type;
            break;
        }
    }

    if (type.empty())
    {
        throw std::invalid_argument("Unsupported directive: " + name);
    }

    return create(type, name, arg);
}

std::unique_ptr<ADirective> DirectiveFactory::create(std::string_view type, const std::string &name,
                                                     const std::string &arg)
{
    const auto &factories = getFactories();
    auto it = factories.find(type);
    if (it == factories.end())
    {
        throw std::invalid_argument("No factory found for directive type: " + std::string(type));
    }
    return it->second(name, utils::trimSemi(utils::trim(arg)));
}

const std::unordered_map<std::string_view, DirectiveFactory::CreatorFunc> &DirectiveFactory::getFactories()
{
    static const std::unordered_map<std::string_view, CreatorFunc> factories = {
        {"BoolDirective",
         [](const std::string &name, const std::string &arg) { return std::make_unique<BoolDirective>(name, arg); }},
        {"IntDirective",
         [](const std::string &name, const std::string &arg) { return std::make_unique<IntDirective>(name, arg); }},
        {"SizeDirective",
         [](const std::string &name, const std::string &arg) { return std::make_unique<SizeDirective>(name, arg); }},
        {"StringDirective",
         [](const std::string &name, const std::string &arg) { return std::make_unique<StringDirective>(name, arg); }},
        {"IntStringDirective", [](const std::string &name,
                                  const std::string &arg) { return std::make_unique<IntStringDirective>(name, arg); }},
        {"VectorDirective",
         [](const std::string &name, const std::string &arg) { return std::make_unique<VectorDirective>(name, arg); }},
    };
    return factories;
}
