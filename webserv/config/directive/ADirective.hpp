#pragma once

#include <any>
#include <array>
#include <string>

class DirectiveValue
{
  public:
    DirectiveValue(std::any value) : value_(value) {}

    template <typename T> operator T() const { return std::any_cast<T>(value_); }

  private:
    std::any value_;
};

class ADirective
{
  public:
    ADirective() = delete;

    ADirective(std::string name) : name_(std::move(name)) {}

    ADirective(const ADirective &other) = delete;
    ADirective &operator=(const ADirective &other) = delete;
    ADirective(ADirective &&other) noexcept = delete;
    ADirective &operator=(ADirective &&other) noexcept = delete;

    virtual ~ADirective() {}

    virtual void parse(const std::string &value) = 0;

    [[nodiscard]] virtual std::any getValue() const = 0;

    [[nodiscard]] DirectiveValue get() const;

    [[nodiscard]] std::string getName() const;

    void setName(const std::string &name);

  protected:
    std::string name_;
};

// Supported directives:

// - listen INT {server}
// - host STRING {server}
// - server_name STRING {server}
// - root STRING {server, location}
// - index STRING[] {server, location}
// - error_page INT STIRNG {server, location}
// - client_max_body_size SIZE {server, location}
// - autoindex BOOL {location}
// - allowed_methods STRING[] {location}
// - cgi_pass STRING {location}
// - cgi_ext STRING[] {location}
// - cgi_timout INT {location}
// - upload_enabled BOOL {location}
// - upload_store STRING {location}
// - redirect INT STRING {location}