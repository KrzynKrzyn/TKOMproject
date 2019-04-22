#include "headers/error.hpp"

std::map<Error::Type, Error::Severity> Error::error_level =
{
    //{Error::Type::Unknown_symbol, Error::Severity::Warning}
    {Error::Type::File_non_existent, Error::Severity::Critical}
};

Error::Severity Error::getSeverity() const
{
    auto severity = error_level.find(type);
    if(severity == error_level.end()) return Severity::Critical;

    return severity->second;
}
