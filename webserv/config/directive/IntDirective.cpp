#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType
#include <webserv/config/directive/IntDirective.hpp>   // for IntDirective
#include <webserv/log/Log.hpp>                       // for Log

IntDirective::IntDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
{
    parse(value);
}

void IntDirective::parse(const std::string &value)
{
    try
    {
        value_ = std::stoi(value);
        Log::debug("IntDirective: parsed integer value " + std::to_string(value_) + " from string \"" + value + "\"");
    }
    catch (const std::invalid_argument &e)
    {
        throw std::invalid_argument("numeric value expected");
    }
    catch (const std::out_of_range &e)
    {
        throw std::invalid_argument("IntDirective: integer out of range");
    }
}

DirectiveValueType IntDirective::getValueType() const
{
    return value_;
}