#include <iostream>

#include "lexer.hpp"

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

    Lexer lex("example.txt");

    Token t = lex.getToken();
    while(t.getType() != Token::Type::Broken)
    {
        t = lex.getToken();
    }

    return 0;
}
