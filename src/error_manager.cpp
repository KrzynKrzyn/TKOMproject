#include "error_manager.hpp"
#include <algorithm>

std::unordered_map<Error::Type, std::string, EnumClassHash> ErrorManager::error_message =
    {
        {Error::Type::File_non_existent, "File doesn\'t exist!"},
        {Error::Type::Bad_identifier, "Invalid identifier!"},
        {Error::Type::Unknown_symbol, "Unknown symbol!"},
        {Error::Type::Unexpected_token, "Unexpected token!"},
        {Error::Type::Uninitialized_variable, "Undeclared variable!"},
        {Error::Type::Uninitialized_function, "Undeclared function!"},
        {Error::Type::Uninitialized_class, "Undeclared class!"},
        {Error::Type::Multi_initialization, "Multiple declaration!"},
        {Error::Type::Bad_constructor, "Constructor name doesn't match class name!"},
        {Error::Type::Bad_type, "Types doesn't match!"},
        {Error::Type::Bad_return, "Return type doesn't match function type!"},
        {Error::Type::Unused_const, "Unused expression!"},
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

    if(err.getExtraInfo() != std::string()) 
        return message->second + " (" + err.getExtraInfo() + ")";
    else
        return message->second;
}

std::string ErrorManager::getInfo(Error err) const
{
    return std::string("Line: " + std::to_string(err.getLine()) +
                        " \tPosition: " + std::to_string(err.getPosition()) +
                        " \t" + getSeverity(err) + ": " + getMessage(err));    
}

void ErrorManager::handleError(Error err)
{
    if(err.getSeverity() == Error::Severity::Warning)
    {
        noncritical_errors.push_back(err);
        return;
    }

    throw std::runtime_error(getInfo(err));
}

std::vector<std::string> ErrorManager::getWarnings()
{
    std::sort(noncritical_errors.begin(), noncritical_errors.end(), 
        [](Error a, Error b)
        {
            return a.getLine() == b.getLine() ? a.getPosition() < b.getPosition() : a.getLine() < b.getLine();
        });

    std::vector<std::string> ret;
    ret.reserve(noncritical_errors.size());

    while(noncritical_errors.size() > 0)
    {
        ret.push_back(getInfo(noncritical_errors.front()));
        noncritical_errors.pop_front();
    }

    return ret;
}
