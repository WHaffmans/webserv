#pragma once

#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType
#include <string> // for string, basic_string

class IntDirective : public ADirective
{
  public:
    IntDirective() = delete;

    IntDirective(const std::string &name, const std::string &value);

    IntDirective(const IntDirective &other) = delete;
    IntDirective &operator=(const IntDirective &other) = delete;
    IntDirective(IntDirective &&other) noexcept = delete;
    IntDirective &operator=(IntDirective &&other) noexcept = delete;

    ~IntDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValueType() const override;

  private:
    int value_ = 0;
};