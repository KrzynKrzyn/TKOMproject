#include "error.hpp"

std::unordered_map<Error::Type, Error::Severity, EnumClassHash> Error::error_level =
{
    //{Error::Type::Unknown_symbol, Error::Severity::Warning}
};

Error::Severity Error::getSeverity() const
{
    auto severity = error_level.find(type);
    if(severity == error_level.end()) return Severity::Critical;

    return severity->second;
}
