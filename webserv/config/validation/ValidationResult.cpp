#include <webserv/config/validation/ValidationResult.hpp>
#include <webserv/log/Log.hpp> // for Log

#include <utility> // for move

ValidationResult::ValidationResult(Type type, std::string message) : type_(type), message_(std::move(message)) {}

ValidationResult ValidationResult::success()
{
    return {ValidationResult::Type::SUCCESS};
}

ValidationResult ValidationResult::error(const std::string &message)
{
    Log::error(message);
    return {ValidationResult::Type::ERROR, message};
}

ValidationResult ValidationResult::warning(const std::string &message)
{
    Log::warning(message);
    return {ValidationResult::Type::WARNING, message};
}

bool ValidationResult::isValidResult() const noexcept
{
    return type_ == Type::SUCCESS;
}

ValidationResult::Type ValidationResult::getType() const noexcept
{
    return type_;
}

std::string ValidationResult::getMessage() const
{
    return message_;
}