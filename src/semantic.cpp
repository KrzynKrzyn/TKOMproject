#include "semantic.hpp"

void SemanticAnaliser::openScope()
{
    scope.push_back(Scope());
}

void SemanticAnaliser::closeScope()
{
    scope.pop_back();
}

bool SemanticAnaliser::isVarDeclared(std::string variable)
{
    for(Scope s : scope)
        if(s.sym_table.find(variable) != s.sym_table.end()) return true;

    return false;
}

bool SemanticAnaliser::isFuncDeclared(std::string func_proto)
{
    for(Scope s : scope)
        if(s.func_table.find(func_proto) != s.func_table.end()) return true;

    return false;
}

SemanticAnaliser::Func& SemanticAnaliser::newFunc(ast::Node& root)  //root better be constructor(!) or function declaration
{
    Func new_func;
    new_func.name = root.children[0].production.value;
    new_func.type = root.production.value;//TODO type check

    int i = 0;
    while(i < root.children.size() && root.children[i].production.name == "Argument")
        new_func.arg_types.push_back(root.children[i++].production.value);//TODO type check

    if(isFuncDeclared(new_func.getPrototype())) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, root.production.row, root.production.pos));

    scope.back().func_table[new_func.getPrototype()] = new_func;

    return scope.back().func_table[new_func.getPrototype()];
}

SemanticAnaliser::Var& SemanticAnaliser::newVar(ast::Node& root)    //root better be argument or variable declaration
{
    Var new_var;
    new_var.name = root.children[0].production.value;
    new_var.type = root.production.value;//TODO type check

    if(isVarDeclared(new_var.name)) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, root.production.row, root.production.pos));

    scope.back().sym_table[new_var.name] = new_var;

    return scope.back().sym_table[new_var.name];
}

void SemanticAnaliser::checkSemantics(ast::Node &root)
{

}

void SemanticAnaliser::declareFunction(ast::Node &root)
{
    Func& f = newFunc(root);

    openScope();

    for(int i=0; i<f.arg_types.size(); ++i)
        newVar(root.children[i+1]);

    for(int i=f.arg_types.size(); i<root.children.size(); ++i)
        checkSemantics(root.children[i]);

    closeScope();
}

void SemanticAnaliser::declareVariable(ast::Node &root)
{
    Var& v = newVar(root);
}

void SemanticAnaliser::declareClass(ast::Node &n)
{

}

void SemanticAnaliser::analyse()
{
    ast::Node root = std::move(parser.parse());

    for(ast::Node &n : root.children) 
    {
        if(n.production.name == "Class") declareClass(n);
        if(n.production.name == "Function declaration") declareFunction(n);
        if(n.production.name == "Variable declaration") declareVariable(n);
        
    }

}