#include "lexer.hpp"

Token Lexer::getToken()
{
    skipWhitespaces();
    if(source.isEOF()) return Token(Token::Type::Eof, source.getLine(), source.getPosition());

    char c = source.getCharacter();

    if(isDigit(c)) return getNumber();
    if(isLetter(c))
    {
        Token initial = getIdent();
        //use unordered map instead
        if(initial.getName() == "if") return Token(Token::Type::If, initial.getLine(), initial.getPosition());
        else if(initial.getName() == "else") return Token(Token::Type::Else, initial.getLine(), initial.getPosition());
        else if(initial.getName() == "while") return Token(Token::Type::While, initial.getLine(), initial.getPosition());
        else if(initial.getName() == "class") return Token(Token::Type::Class, initial.getLine(), initial.getPosition());
        else if(initial.getName() == "return") return Token(Token::Type::Return, initial.getLine(), initial.getPosition());
        else if(initial.getName() == "true") return Token(Token::Type::Bool, initial.getLine(), initial.getPosition(), true);
        else if(initial.getName() == "false") return Token(Token::Type::Bool, initial.getLine(), initial.getPosition(), false);
        else if(initial.getName() == "private" && source.getCharacter() == ':')
        {
            source.nextChar();
            return Token(Token::Type::Private, initial.getLine(), initial.getPosition());
        }
        else if(initial.getName() == "public" && source.getCharacter() == ':')
        {
            source.nextChar();
            return Token(Token::Type::Public, initial.getLine(), initial.getPosition());
        }

        return initial;
    }

    return getSpecial();
}

Token Lexer::getIdent()
{
    char c = source.getCharacter();
    int line=source.getLine(), pos=source.getPosition();

    std::string name = std::string(1,c);//(1, c);

    c = source.nextChar();
    while(isLetter(c) || isDigit(c))
    {
        name += c;
        c = source.nextChar();
    }

    return Token(Token::Type::Ident, line, pos, name);
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

    if(isLetter(c)) { error_manager.handleError(Error(Error::Type::Bad_identifier, line, pos)); return Token(Token::Type::Broken, line, pos); }
    if(c != '.') return Token(Token::Type::Int, line, pos, i);

    double d = 0, j = 10;
    while(isDigit(c = source.nextChar()))
    {
        double temp = (c - '0') / j;
        d += temp;
        j *= 10;
    }

    if(isLetter(c)) { error_manager.handleError(Error(Error::Type::Bad_identifier, line, pos)); return Token(Token::Type::Broken, line, pos); }
    return Token(Token::Type::Double, line, pos, i+d);
}

Token Lexer::getSpecial()
{
    char c = source.getCharacter();
    int line=source.getLine(), pos=source.getPosition();
    char next = source.nextChar();

    switch(c)
    {
        case '+': return Token(Token::Type::Plus, line, pos); break;
        case '-': return Token(Token::Type::Minus, line, pos); break;
        case '*': return Token(Token::Type::Multiplication, line, pos); break;
        case '/': return Token(Token::Type::Division, line, pos); break;
        case '%': return Token(Token::Type::Modulus, line, pos); break;
        case ';': return Token(Token::Type::Semicolon, line, pos); break;
        case ':': return Token(Token::Type::Colon, line, pos); break;
        case '(': return Token(Token::Type::OpenBracket, line, pos); break;
        case ')': return Token(Token::Type::CloseBracket, line, pos); break;
        case '{': return Token(Token::Type::OpenCurly, line, pos); break;
        case '}': return Token(Token::Type::CloseCurly, line, pos); break;
        case ',': return Token(Token::Type::Comma, line, pos); break;
        case '.': return Token(Token::Type::Dot, line, pos); break;
        case '=':
            if(next == '=')
            {
                source.nextChar();
                return Token(Token::Type::Equals, line, pos);
            }
            else return Token(Token::Type::Assignment, line, pos); break;
        case '>':
            if(next == '=')
            {
                source.nextChar();
                return Token(Token::Type::GEqual, line, pos);
            }
            else return Token(Token::Type::Greater, line, pos); break;
        case '<':
            if(next == '=')
            {
                source.nextChar();
                return Token(Token::Type::LEqual, line, pos);
            }
            else return Token(Token::Type::Less, line, pos); break;
        case '!':
            if(next == '=')
            {
                source.nextChar();
                return Token(Token::Type::NEqual, line, pos);
            }
            else return Token(Token::Type::Not, line, pos); break;
        case '&':
            if(next == '&')
            {
                source.nextChar();
                return Token(Token::Type::And, line, pos);
            }
            else return Token(Token::Type::Broken, line, pos); break;
        case '|':
            if(next == '|')
            {
                source.nextChar();
                return Token(Token::Type::Or, line, pos);
            }
            else return Token(Token::Type::Broken, line, pos); break;
    }

    error_manager.handleError(Error(Error::Type::Unknown_symbol, line, pos));
    return Token(Token::Type::Broken, line, pos);
}

