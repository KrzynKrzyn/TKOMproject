#include "error_manager.hpp"

std::unordered_map<Error::Type, std::string, EnumClassHash> ErrorManager::error_message =
    {
        {Error::Type::Bad_identifier, "Invalid identifier!"},
        {Error::Type::Unknown_symbol, "Unknown symbol!"},
    };
