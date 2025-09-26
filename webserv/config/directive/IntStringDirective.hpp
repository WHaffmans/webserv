#pragma once
#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType
#include <string>  // for basic_string, string
#include <utility> // for pair

class IntStringDirective : public ADirective
{
  public:
    IntStringDirective() = delete;

    IntStringDirective(const std::string &name, const std::string &value);

    IntStringDirective(const IntStringDirective &other) = delete;
    IntStringDirective &operator=(const IntStringDirective &other) = delete;
    IntStringDirective(IntStringDirective &&other) noexcept = delete;
    IntStringDirective &operator=(IntStringDirective &&other) noexcept = delete;

    ~IntStringDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValueType() const override;

  private:
    std::pair<int, std::string> value_ = {0, ""};
};