#include <iostream>

#include "headers/parser.hpp"

//do something with awful page-long switches and ifs
//implement token tostring()
//cleanup include mess
//error handling -> warnings

//arithm_ele and bool_ele -> to be changed
//expr / bool_expr -> to be changed

using namespace std;

void printTree(ast::Node n, int level)
{
    for(int i = 0; i<level; ++i) std::cout << "│";
    std::cout << static_cast<int>(n.type) << std::endl;

    for(ast::Node ni : n.children) 
    {
        for(int i = 0; i<level; ++i) std::cout << "│";
        std::cout << "├" << "┐" << std::endl;
        printTree(ni, level + 1);
    }

    //for(int i = 0; i<level; ++i) std::cout << "|";
}

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

    ast::Node tree;

    try {tree = par.parse();}
    catch(std::runtime_error e)
    {
        cout << e.what() << endl;
        return 1;
    }
    
    printTree(tree, 0);
    std::cout << std::endl;

    return 0;
}
