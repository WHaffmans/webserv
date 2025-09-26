#pragma once

#include "webserv/config/directive/DirectiveValue.hpp"

#include <webserv/config/directive/ADirective.hpp> // for ADirective

#include <map>
#include <memory>
#include <string>

class AConfig
{
  public:
    AConfig() = default;
    AConfig(const AConfig *parent);
    AConfig(const AConfig &other) = delete;
    AConfig &operator=(const AConfig &other) = delete;
    AConfig(AConfig &&other) noexcept = delete;
    AConfig &operator=(AConfig &&other) noexcept = delete;

    virtual ~AConfig() = default;

    void addDirective(const std::string &line);
    [[nodiscard]] const ADirective *getDirective(const std::string &name) const;

    [[nodiscard]] bool hasDirective(const std::string &name) const;

    template <typename T> T getDirectiveValue(const std::string &name, const T &defaultValue = T{}) const
    {
        const auto *directive = getDirective(name);
        if (!directive)
        {
            return defaultValue;
        }

        auto value = directive->getValue();
        if (value.holds<T>())
        {
            return value.get<T>();
        }
        return defaultValue;
    }

  protected:
    virtual void parseBlock(const std::string &block) = 0;
    void parseDirectives(const std::string &declarations);
    std::map<std::string, std::unique_ptr<ADirective>>
        directives_;                  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
    const AConfig *parent_ = nullptr; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};