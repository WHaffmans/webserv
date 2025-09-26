#pragma once

#include <webserv/config/directive/ADirective.hpp>     // for ADirective
#include <webserv/config/directive/DirectiveValue.hpp> // for DirectiveValueType

#include <string> // for basic_string, string
#include <vector> // for vector

class VectorDirective : public ADirective
{
  public:
    VectorDirective() = delete;

    VectorDirective(const std::string &name, const std::string &value);

    VectorDirective(const VectorDirective &other) = delete;
    VectorDirective &operator=(const VectorDirective &other) = delete;
    VectorDirective(VectorDirective &&other) noexcept = delete;
    VectorDirective &operator=(VectorDirective &&other) noexcept = delete;

    ~VectorDirective() override = default;

    void parse(const std::string &value) override;

    [[nodiscard]] DirectiveValueType getValueType() const override;

  private:
    std::vector<std::string> value_;
};