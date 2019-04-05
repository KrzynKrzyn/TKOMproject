#include <iostream>

#include "lexer.hpp"

//introduce token::eof instead of token::broken
//do something with awful page-long switches and ifs
//implement token tostring()

using namespace std;

int main()
{/*
    FileSource blob("example.txt");

    while(!blob.isEOF())
    {
        cout << blob.nextChar();
        cout << '\t' << blob.getLine() << ":" << blob.getPosition() << endl;
    }

    char test = 'a'; //48 - 57

    cout << test << endl;*/

    Lexer lex("example2.txt");

    Token t = lex.getToken();
    while(t.getType() != Token::Type::Broken && t.getType() != Token::Type::Eof)
    {
        t = lex.getToken();
    }

    return 0;
}
