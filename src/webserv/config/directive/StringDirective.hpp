#pragma once

#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType

#include <string> // for string, basic_string

class StringDirective : public ADirective
{
  public:
    StringDirective() = delete;

    StringDirective(const std::string &name, const std::string &value);
    StringDirective(const StringDirective &other) = delete;
    StringDirective &operator=(const StringDirective &other) = delete;
    StringDirective(StringDirective &&other) noexcept = delete;
    StringDirective &operator=(StringDirective &&other) noexcept = delete;

    ~StringDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValueType() const override;

  private:
    std::string value_;
};