#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <unordered_map>

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

class Error
{
    public:
        enum class Type
        {
            Bad_identifier,
            Unknown_symbol,
        };

        enum class Severity
        {
            Critical,
            Warning
        };

    private:
        Type type;
        int line;
        int position;

        static std::unordered_map<Error::Type, Error::Severity, EnumClassHash> error_level;

    public:
        Error(Type type_, int line_, int position_): type(type_), line(line_), position(position_) {}

        Type getType() const { return type; }
        Severity getSeverity() const;

        int getPosition() const { return position; }
        int getLine() const { return line; }
};



#endif // ERROR_HPP_INCLUDED
