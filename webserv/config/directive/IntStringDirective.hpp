#pragma once

#include "ADirective.hpp"

#include <any>

class IntStringDirective : public ADirective
{
  public:
    IntStringDirective() = delete;

    IntStringDirective(const std::string &name, const std::string &value) : ADirective(name) { parse(value); }

    IntStringDirective(const IntStringDirective &other) = delete;
    IntStringDirective &operator=(const IntStringDirective &other) = delete;
    IntStringDirective(IntStringDirective &&other) noexcept = delete;
    IntStringDirective &operator=(IntStringDirective &&other) noexcept = delete;

    ~IntStringDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValue() const override;

  private:
    std::pair<int, std::string> value_ = {0, ""};
};