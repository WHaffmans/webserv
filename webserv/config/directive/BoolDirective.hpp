#pragma once

#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType

#include <string> // for string, basic_string

class BoolDirective : public ADirective
{
  public:
    BoolDirective() = delete;

    BoolDirective(const std::string &name, const std::string &value);

    BoolDirective(const BoolDirective &other) = delete;
    BoolDirective &operator=(const BoolDirective &other) = delete;
    BoolDirective(BoolDirective &&other) noexcept = delete;
    BoolDirective &operator=(BoolDirective &&other) noexcept = delete;

    ~BoolDirective() override = default;

    void parse(const std::string &arg) override;

    [[nodiscard]] DirectiveValueType getValueType() const override;

  private:
    bool value_ = false;
};