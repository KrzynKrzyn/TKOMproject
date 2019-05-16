#include "error.hpp"

std::map<Error::Type, Error::Severity> Error::error_level =
{
    //{Error::Type::Unknown_symbol, Error::Severity::Warning}
    {Error::Type::File_non_existent, Error::Severity::Critical},
    {Error::Type::Unused_const, Error::Severity::Warning},
    {Error::Type::Unused_variable, Error::Severity::Warning},
    {Error::Type::Unused_function, Error::Severity::Warning},
    {Error::Type::Unused_constructor, Error::Severity::Warning},
    {Error::Type::Unused_class, Error::Severity::Warning},
};

Error::Severity Error::getSeverity() const
{
    auto severity = error_level.find(type);
    if(severity == error_level.end()) return Severity::Ordinary;

    return severity->second;
}
