#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include "source.hpp"
#include "token.hpp"

class Lexer
{
    private:
        FileSource source;

    public:
        Token getToken();

        Lexer(std::string filename);
};

#endif // LEXER_HPP_INCLUDED
