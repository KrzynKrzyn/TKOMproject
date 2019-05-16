#include "error_manager.hpp"

std::unordered_map<Error::Type, std::string, EnumClassHash> ErrorManager::error_message =
    {
        {Error::Type::File_non_existent, "File doesn\'t exist!"},
        {Error::Type::Bad_identifier, "Invalid identifier!"},
        {Error::Type::Unknown_symbol, "Unknown symbol!"},
        {Error::Type::Unexpected_token, "Unexpected token!"},
        {Error::Type::Uninitialized_variable, "Uninitialized variable!"},
        {Error::Type::Uninitialized_function, "Uninitialized function!"},
        {Error::Type::Uninitialized_class, "Uninitialized class!"},
        {Error::Type::Multi_initialization, "Multiple initialization!"},
        {Error::Type::Unused_const, "Unused const!"},
        {Error::Type::Unused_variable, "Unused variable!"},
        {Error::Type::Unused_function, "Unused function!"},
        {Error::Type::Unused_constructor, "Unused constructor!"},
        {Error::Type::Unused_class, "Unused class!"},
    };

std::string ErrorManager::getSeverity(Error err) const
{
    if(err.getSeverity() == Error::Severity::Warning) return "Warning";
    else if(err.getSeverity() == Error::Severity::Ordinary) return "Error";
    else if(err.getSeverity() == Error::Severity::Critical) return "Critical";

    return "Error";
}

std::string ErrorManager::getMessage(Error err) const
{
    auto message = error_message.find(err.getType());
    if(message == error_message.end()) return "Unknown error";

    return message->second;
}

std::string ErrorManager::getInfo(Error err) const
{
    return std::string("Line: " + std::to_string(err.getLine()) +
                        "\tPosition: " + std::to_string(err.getPosition()) +
                        "\t" + getSeverity(err) + ": " + getMessage(err));    
}

void ErrorManager::handleError(Error err)
{
    if(err.getSeverity() == Error::Severity::Warning)
    {
        noncritical_errors.push(err);
        return;
    }

    throw std::runtime_error(getInfo(err));
}

std::vector<std::string> ErrorManager::getWarnings()
{
    std::vector<std::string> ret(noncritical_errors.size());

    while(noncritical_errors.size() > 0)
    {
        ret.push_back(getInfo(noncritical_errors.back()));
        noncritical_errors.pop();
    }

    return ret;
}
