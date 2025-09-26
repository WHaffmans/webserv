#include <webserv/config/directive/IntStringDirective.hpp> // for IntDirective
#include <webserv/config/utils.hpp>                        // for trim

#include <sstream> // for basic_istringstream, basic_istream::operator>>, istringstream

IntStringDirective::IntStringDirective(const std::string &name, const std::string &value)
    : ADirective(name) // NOLINT(bugprone-easily-swappable-parameters)
{
    parse(value);
}

void IntStringDirective::parse(const std::string &value)
{
    std::istringstream iss(value);
    int intPart = 0;
    std::string strPart;
    iss >> intPart;
    std::getline(iss, strPart);
    strPart = utils::trim(strPart); // Remove leading space
    value_ = std::make_pair(intPart, strPart);
}

DirectiveValueType IntStringDirective::getValueType() const
{
    return value_;
}