#ifndef ERROR_HPP_INCLUDED
#define ERROR_HPP_INCLUDED

class Error
{
    public:
        enum class Type
        {
            Bad_identifier,
            Unknown_symbol,
        };

    private:
        Type type;
        int line;
        int position;

    public:
        Error(Type type_, int line_, int position_): type(type_), line(line_), position(position_) {}

        Type getType() const { return type; }
        int getPosition() const { return position; }
        int getLine() const { return line; }
};

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

#endif // ERROR_HPP_INCLUDED
