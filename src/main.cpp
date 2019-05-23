#include <iostream>
#include <sys/stat.h>

#include "semantic.hpp"

//arithm_ele and bool_ele -> to be changed
//expr / bool_expr -> to be changed

//type handling in ast -- to be thought through
//backtracking -> one parser member variable instead of current joggling between methods

//check duplicates -> wrong error number
//delete arg_types

using namespace std;

inline bool file_exists(const std::string& name) 
{
  struct stat buffer;   
  return (stat(name.c_str(), &buffer) == 0); 
}

int main(int argc, char* argv[])
{
    bool par_info = false, sem_info = false, input_correct = true;

    if(argc < 2 || argc > 4) input_correct = false;
    else if(argc == 3)
    {
        if(std::string(argv[1]) == "-p") par_info = true;
        if(std::string(argv[1]) == "-s") sem_info = true;

        if(!par_info && !sem_info) input_correct = false;
    }
    else if(argc == 4)
    {
        if(std::string(argv[1]) == "-p" || std::string(argv[2]) == "-p") par_info = true;
        if(std::string(argv[1]) == "-s" || std::string(argv[2]) == "-s") sem_info = true;

        if(!par_info || !sem_info) input_correct = false;
    }

    if(!input_correct)
    {
        cout << "Usage:" << endl;
        cout << ' ' << argv[0] << " [-p][-s]" << " filename" << endl;
        cout << '\t' << "-p" << " - print abstract syntax tree" << endl;
        cout << '\t' << "-s" << " - print semantic analysis result" << endl;

        return 1;
    }    

    std::string filename(argv[argc-1]);

    if(!file_exists(filename))
    {
        cout << "File " << filename << " doesn't exists!" <<endl;

        return 1;
    }

    ErrorManager err_man;
    FileSource source(filename, err_man);
    Lexer lex(source, err_man);
    Parser par(lex, err_man);
    SemanticAnaliser sem(par, err_man);
    
    try 
    {
        sem.analyse();
    }
    catch(std::runtime_error e)
    {
        cout << e.what() << endl;

        std::vector<std::string> warnings = err_man.getWarnings();
        for(std::string w : warnings) cout << w << endl;

        std::cout << std::endl << "Analysis finished, errors has been found!" << std::endl;
        return 1;
    }
    
    if(par_info) par.printAST();
    if(sem_info) sem.printAnaliseInfo();

    std::vector<std::string> war = err_man.getWarnings();
    for(std::string s : war)
        std::cout << s << std::endl;

    std::string message = (war.size() > 0) ? ", only warnings has been found." : ", no errors has been found.";
    std::cout << std::endl << "Analysis finished" << message << std::endl;

    return 0;
}
