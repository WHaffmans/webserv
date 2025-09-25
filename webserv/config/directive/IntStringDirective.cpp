#include "webserv/log/Log.hpp"
#include <webserv/config/directive/IntStringDirective.hpp> // for IntDirective
#include <webserv/config/utils.hpp>                     // for trim
#include <sstream>                                     // for std::getline, std::basic_istream, std::char_traits, std::basic_stringbuf


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

DirectiveValueType IntStringDirective::getValue() const
{
    return value_;
}