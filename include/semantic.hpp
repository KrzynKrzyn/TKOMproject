#ifndef SEMANTIC_HPP_INCLUDED
#define SEMANTIC_HPP_INCLUDED

#include <map>
#include <vector>
#include "error_manager.hpp"
#include "parser.hpp"

class SemanticAnaliser
{
    private:
        Parser &parser;
        ErrorManager &error_manager;

        struct Symbol
        {
            std::string name;
            std::string type;

            int usage_count = 0;
        };

        struct Func : Symbol
        {
            std::vector<std::string> arg_types;

            std::string getPrototype()
            {
                std::string func_proto = name + "(";

                for(std::string arg_type : arg_types)
                    func_proto = func_proto + arg_type + ",";

                func_proto.pop_back();
                func_proto = func_proto + ")";

                return func_proto;
            }
        };

        struct Var : Symbol
        {

        };

        struct Scope
        {
            std::map<std::string, Var> sym_table;
            std::map<std::string, Func> func_table;
        };

        struct Class : Symbol
        {
            Scope public_scope, private_scope;
        };

        std::map<std::string, Class> type_table;
        std::vector<Scope> scope;

        void openScope();
        void closeScope();

        bool isVarDeclared(std::string variable);
        bool isFuncDeclared(std::string func_proto);
        bool isClassDeclared(std::string class_name);

        Func& newFunc(ast::Node& root);  //root better be constructor(!) or function declaration
        Var& newVar(ast::Node& root);    //root better be argument or variable declaration
        Class& newClass(ast::Node& root);

        void checkSemantics(ast::Node &root);

    public:
        /*bool isClassDeclared(std::string class_name)
        {
            return (type_table.find(class_name) != type_table.end());
        }*/
        
        void declareFunction(ast::Node &root);
        void declareVariable(ast::Node &root);
        void declareClass(ast::Node &root);

        void analyse();

        SemanticAnaliser(Parser &p, ErrorManager &em): parser(p), error_manager(em) {}
};
/*
class SemanticAnaliser
{
    private:
        ErrorManager &error_manager;

        struct Symbol
        {
            enum class Construction
            {
                s_variable,
                s_function,
                s_constructor,
            };

            std::string type;
            std::string name;
            Construction construction;

            int usage_count;
        };

        struct Class
        {
            std::string name;
            int usage_count;

            std::map<std::string, Symbol> sym_table;
        };

        std::map<std::string, Class> type_table;
        std::vector< std::map<std::string, Symbol> > sym_table;

    public:
        SemanticAnaliser(ErrorManager &em): error_manager(em) {}
};
*/
#endif // SEMANTIC_HPP_INCLUDED