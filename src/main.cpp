#include <iostream>

#include "parser.hpp"
#include "semantic.hpp"

//arithm_ele and bool_ele -> to be changed
//expr / bool_expr -> to be changed

//type handling in ast -- to be thought through
//backtracking -> one parser member variable instead of current joggling between methods

//check duplicates -> wrong error number
//delete arg_types

using namespace std;

int main(int argc, char* argv[])
{
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
    SemanticAnaliser sem(par, err_man);
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
        sem.analyse();
    }
    catch(std::runtime_error e)
    {
        cout << e.what() << endl;
        std::vector<std::string> warnings = err_man.getWarnings();
        for(std::string w : warnings) cout << w << endl;
        return 1;
    }
    
    return 0;
}
