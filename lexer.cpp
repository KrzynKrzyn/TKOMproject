#include "lexer.hpp"

Token Lexer::getToken()
{
    if(skipWhitespaces() == std::char_traits<char>::eof())
        return Token(Token::Type::Broken, source.getLine(), source.getPosition());

    char c = source.getCharacter();

    if(isDigit(c)) return getNumber();
    if(isLetter(c)) return getIdent();

    return Token(Token::Type::Broken, source.getLine(), source.getPosition());
}

Token Lexer::getIdent()
{

}

Token Lexer::getNumber()
{
    char c = source.getCharacter();
    int line=source.getLine(), pos=source.getPosition();

    int i = c - '0';
    while(isDigit(c = source.nextChar()))
    {
        i *= 10;
        i += (c - '0');
    }

    if(c != '.')
    {
        source.nextChar();
        return Token(Token::Type::Int, line, pos, i);
    }

    double d = 0;
    double j = 10;
    while(isDigit(c = source.nextChar()))
    {
        double temp = (c - '0') / j;
        d += temp;
        j *= 10;
    }

    return Token(Token::Type::Double, line, pos, i+d);
}

