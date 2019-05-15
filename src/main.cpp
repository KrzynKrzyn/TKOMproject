#include <iostream>

#include "headers/parser.hpp"

//do something with awful page-long switches and ifs
//implement token tostring()
//cleanup include mess
//error handling -> warnings

//arithm_ele and bool_ele -> to be changed
//expr / bool_expr -> to be changed

//type handling in ast -- to be thought through

using namespace std;

int main(int argc, char* argv[])
{/*
    FileSource blob("example.txt");

    while(!blob.isEOF())
    {
        cout << blob.nextChar();
        cout << '\t' << blob.getLine() << ":" << blob.getPosition() << endl;
    }

    char test = 'a'; //48 - 57

    cout << test << endl;*/

    if(argc != 2)
    {
        cout << "Usage:" << endl;
        cout << ' ' << argv[0] << " filename" << endl;
        return 1;
    }
    std::string filename(argv[1]);

    ErrorManager err_man;
    FileSource source(filename, err_man);
    Lexer lex(source, err_man);
    Parser par(lex, err_man);
/*
    Token t;// = lex.getToken();
    do
    {
        try {t = lex.getToken();}
        catch(std::runtime_error e)
        {
            cout << e.what() << endl;
            return 1;
        }

        cout << t.getLine() << ':' << t.getPosition() << " -- " << t.toString() << endl;
    }
    while(t.getType() != Token::Type::Broken && t.getType() != Token::Type::Eof);*/
    try 
    {
        ast::Node root = par.parse();
        root.adjustLevels();
        root.printTree();
    }
    catch(std::runtime_error e)
    {
        cout << e.what() << endl;
        return 1;
    }
    

    return 0;
}
