#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

#include <map>

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
            File_non_existent,
            Bad_identifier,
            Unknown_symbol,
            Unexpected_token,
            Uninitialized_variable,
            Uninitialized_function,
            Uninitialized_class,
            Multi_initialization,
            Bad_constructor,
            Bad_type,
            Bad_return,
            Unused_const,
            Unused_variable,
            Unused_function,
            Unused_constructor,
            Unused_class,
        };

        enum class Severity
        {
            Critical,
            Ordinary,
            Warning
        };

    private:
        Type type;
        int line;
        int position;

        std::string extra_info;

        static std::map<Error::Type, Error::Severity> error_level;

    public:
        //Error(Type type_, int line_, int position_):
        //    type(type_), line(line_), position(position_) {}
        Error(Type type_, int line_, int position_, std::string extra_info_ = std::string()):
            type(type_), line(line_), position(position_), extra_info(extra_info_) {}

        Type getType() const { return type; }
        Severity getSeverity() const;
        std::string getExtraInfo() const { return extra_info; }

        int getPosition() const { return position; }
        int getLine() const { return line; }
};



#endif // ERROR_HPP_INCLUDED
