#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include <ctype.h>
#include <string>
#include <unordered_map>
#include "source.hpp"
#include "token.hpp"
#include "error_manager.hpp"
#include "error.hpp"

class Lexer
{
    private:
        Source &source;
        ErrorManager &error_manager;

        std::unordered_map<std::string, Token::Type> keywords =
        {
            {"if",      Token::Type::If},
            {"else",    Token::Type::Else},
            {"while",   Token::Type::While},
            {"class",   Token::Type::Class},
            {"return",  Token::Type::Return},
            {"true",    Token::Type::Bool},
            {"false",   Token::Type::Bool},
            {"private", Token::Type::Private},
            {"public",  Token::Type::Public},
        };
/*
        char special[19] =
        {
            '>', '<', '=', '*', '/', '+', '-', ';', '(', ')', '{', '}', ',', '.', '!', '%', '&', '|'
        };*/
        bool isDigit(char c) const
        {
            return c >= '0' && c <= '9';
        }

        bool isLetter(char c) const
        {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
        }

        char skipWhitespaces()
        {
            while(!source.isEOF() && isspace(source.getCharacter())) source.nextChar();
            return source.getCharacter();
        }

        Token getNumber();
        Token getIdent();
        Token getSpecial();

    public:
        Token getToken();


/*
        bool isSpecial(char c) const
        {
            for(int i=0; i<19; ++i) if(c == special[i]) return true;
            return false;
        }*/

        Lexer(Source& s, ErrorManager &em):
            source(s), error_manager(em) { source.nextChar(); }
};

#endif // LEXER_HPP_INCLUDED
