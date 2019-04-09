#include <iostream>

#include "lexer.hpp"

//do something with awful page-long switches and ifs
//implement token tostring()
//cleanup include mess
//error handling -> warnings

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

    FileSource source("example.txt");
    ErrorManager err_man;
    Lexer lex(source, err_man);

    Token t = lex.getToken();
    while(t.getType() != Token::Type::Broken && t.getType() != Token::Type::Eof)
    {
        try {t = lex.getToken();}
        catch(std::runtime_error e)
        {
            cout << e.what() << endl;
            return 1;
        }

        cout << t.toString() << endl;
    }

    return 0;
}
