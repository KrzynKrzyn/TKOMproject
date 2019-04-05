#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include <ctype.h>
#include <string>
#include "source.hpp"
#include "token.hpp"

class Lexer
{
    private:
        FileSource source;

        /*std::string keywords[9] =
        {
            "if", "else", "while",
            "class", "public:", "private:",
            "return", "true", "false"
        };

        char special[19] =
        {
            '>', '<', '=', '*', '/', '+', '-', ';', '(', ')', '{', '}', ',', '.', '!', '%', '&', '|'
        };*/

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

        bool isDigit(char c) const
        {
            return c >= '0' && c <= '9';
        }

        bool isLetter(char c) const
        {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
        }
/*
        bool isSpecial(char c) const
        {
            for(int i=0; i<19; ++i) if(c == special[i]) return true;
            return false;
        }*/

        Lexer(std::string filename):
            source(filename) { source.nextChar(); }
};

#endif // LEXER_HPP_INCLUDED
