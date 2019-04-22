#include "headers/error_manager.hpp"

std::unordered_map<Error::Type, std::string, EnumClassHash> ErrorManager::error_message =
    {
        {Error::Type::File_non_existent, "File doesn\'t exist!"},
        {Error::Type::Bad_identifier, "Invalid identifier!"},
        {Error::Type::Unknown_symbol, "Unknown symbol!"},
    };

std::string ErrorManager::getMessage(Error err) const
{
    auto message = error_message.find(err.getType());
    if(message == error_message.end()) return "Unknown error";

    return message->second;
}

void ErrorManager::handleError(Error err)
{
    if(err.getSeverity() == Error::Severity::Warning)
    {
        noncritical_errors.push(err);
        return;
    }

    throw std::runtime_error("Line: " + std::to_string(err.getLine()) +
                             "\tPosition: " + std::to_string(err.getPosition()) +
                             "\tError: " + getMessage(err));
}
