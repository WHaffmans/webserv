#pragma once

#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType
#include <cstddef> // for size_t
#include <string>  // for string, basic_string

class SizeDirective : public ADirective
{
  public:
    SizeDirective() = delete;

    SizeDirective(const std::string &name, const std::string &value);

    SizeDirective(const SizeDirective &other) = delete;
    SizeDirective &operator=(const SizeDirective &other) = delete;
    SizeDirective(SizeDirective &&other) noexcept = delete;
    SizeDirective &operator=(SizeDirective &&other) noexcept = delete;

    ~SizeDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValueType() const override;

  private:
    size_t value_ = 0;
};