#ifndef TOKEN_HPP_INCLUDED
#define TOKEN_HPP_INCLUDED

class Token
{
    public:
        enum class Type
        {
            Ident,
            Int,
            Double,
            Bool,

            Semicolon,
            Eof,
        };

    private:
        Type type;
        int line;
        int position;

        std::string name;
        union
        {
            int integer;
            double floating;
            bool boolean;
        } value;

    public:
        Token(Type type_, int line_, int position_)                 : type(type_), line(line_), position(position_) {}
        Token(Type type, int line, int position, std::string val)   : Token(type, line, position) { name = val; }
        Token(Type type, int line, int position, int val)           : Token(type, line, position) { value.integer = val; }
        Token(Type type, int line, int position, double val)        : Token(type, line, position) { value.floating = val; }
        Token(Type type, int line, int position, bool val)          : Token(type, line, position) { value.boolean = val; }

        Type getType() const { return type; }
        int getPosition() const { return position; }
        int getLine() const { return line; }

        std::string getName() const { return name; }
        int getInt() const { return value.integer; }
        double getDouble() const { return value.floating; }
        bool getBool() const { return value.boolean; }
};

#endif // TOKEN_HPP_INCLUDED
