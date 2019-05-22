#ifndef SEMANTIC_HPP_INCLUDED
#define SEMANTIC_HPP_INCLUDED

#include <map>
#include <set>
#include <vector>
#include "error_manager.hpp"
#include "parser.hpp"

//checkfor might need separation

    struct Symbol
    {
        std::string name;
        //std::string type;

        int line, pos;

        int usage_count = 0;
/*
        Symbol& operator=(const Symbol&) = default;

        Symbol() = default;
        Symbol(Symbol&) = default;
        Symbol(Symbol&&) = default;*/
        Symbol() {}
        Symbol(std::string name_, int line_, int pos_): name(name_), line(line_), pos(pos_) {}
    };

    struct Var : Symbol
    {
        std::string type;
    };

    struct Func : Symbol
    {
        std::string type;
        std::vector<std::string> arg_types;

        std::string getPrototype() const
        {
            std::string func_proto = name + "(";

            for(std::string arg_type : arg_types)
                func_proto = func_proto + arg_type + ",";

            if(arg_types.size() > 0) func_proto.pop_back();
            func_proto = func_proto + ")";

            return func_proto;
        }
    };

    struct Class : Symbol
    {
        std::map<std::string, Var> class_vars;
        std::map<std::string, Func> class_funcs;

        std::map<std::string, Var> private_vars;
        std::map<std::string, Func> private_funcs;

        Class() = default;

        Class(std::string name_)
        {
            name = name_;
        }
    };

class SemanticAnaliser
{
    private:
        Parser &parser;
        ErrorManager &error_manager;

        std::map<std::string, Var> global_vars;
        std::map<std::string, Func> global_funcs;
        std::map<std::string, Class> classes;
        std::vector<std::map<std::string, Var>> scope_stack;

        std::string private_access = std::string();
        std::string function_return = std::string();

        void openScope();
        void closeScope();

        void checkDuplicates(Symbol sym, std::vector<std::map<std::string, Var>>& symbols);
        void checkDuplicates(Symbol sym, std::map<std::string, Var>& symbols);
        void checkDuplicates(Symbol sym, std::map<std::string, Func>& symbols);
        void checkDuplicates(Symbol sym, std::map<std::string, Class>& symbols);

        std::map<std::string, Var>::iterator getMemberVar(Symbol sym, std::string class_name);
        std::map<std::string, Var>::iterator getVar(Symbol sym, std::string class_name = std::string());
        std::map<std::string, Func>::iterator getFunc(Symbol sym, std::string class_name = std::string());
        std::map<std::string, Class>::iterator getClass(Symbol class_name);

        void declareVar(ast::Node &root, bool priv = false);
        void declareFunc(ast::Node &root, bool priv = false);
        void declareClass(ast::Node &root);

        std::string extractFuncClass(ast::Node &root);

        std::string checkTypeUniformity(ast::Node &root);
        std::string checkType(ast::Node &root, std::string type_name);
        std::string checkFunction(ast::Node &root);
        std::string checkConstructor(ast::Node &root);
        std::string checkVar(ast::Node &root);

        void checkSemantics(ast::Node &root);

    public:
        void analyse();

        SemanticAnaliser(Parser &p, ErrorManager &em): parser(p), error_manager(em) 
        {
            classes = {
                {"void", Class("void")},    //WAT?
                {"int", Class("int")},
                {"double", Class("double")},
                {"bool", Class("bool")}
                };
        }
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