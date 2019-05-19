#include "semantic.hpp"
/*
std::map<std::string, Var>::iterator Scope::insert(Var&& v)
{
    std::string id = v.name;
    sym_table[id] = std::move(v);   //change to insert later on

    return sym_table.find(id);
}

std::map<std::string, Func>::iterator Scope::insert(Func&& f)
{
    std::string id = f.getPrototype();
    func_table[id] = std::move(f);

    return func_table.find(id);
}*/

void SemanticAnaliser::openScope()
{
    scope_stack.push_back(std::map<std::string,Var>());
}

void SemanticAnaliser::closeScope()
{
    for(const auto &v : scope_stack.back())
    if(v.second.usage_count == 0) 
        error_manager.handleError(Error(Error::Type::Unused_variable, v.second.line, v.second.pos));

    std::cout << "Closed scope vars: " << std::endl;
    for(auto& i : scope_stack.back()) std::cout << '\t' << i.second.type << " " << i.second.name << std::endl;
    std::cout << std::endl;

    scope_stack.pop_back();
}

bool SemanticAnaliser::findDecl(std::string sym, std::map<std::string,Symbol> &smap)
{
    return smap.find(sym) != smap.end();
}

void SemanticAnaliser::checkDuplicates(std::string sym, std::vector<std::map<std::string, Var>>& symbols)
{
    for(std::map<std::string, Var>& smap : symbols)
        checkDuplicates(sym, smap);
}

void SemanticAnaliser::checkDuplicates(std::string sym, std::map<std::string, Var>& symbols)    //triple repetition + bad error pos TODO
{
    auto found = symbols.find(sym);
    if(found != symbols.end()) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, found->second.line, found->second.pos));  
}

void SemanticAnaliser::checkDuplicates(std::string sym, std::map<std::string, Func>& symbols)
{
    auto found = symbols.find(sym);
    if(found != symbols.end()) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, found->second.line, found->second.pos));  
}

void SemanticAnaliser::checkDuplicates(std::string sym, std::map<std::string, Class>& symbols)
{
    auto found = symbols.find(sym);
    if(found != symbols.end()) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, found->second.line, found->second.pos));  
}

/*
void SemanticAnaliser::checkDuplicates(std::string sym, std::vector<std::map<std::string, Symbol>>& symbols)
{
    for(std::map<std::string, Symbol>& smap : symbols)
        checkDuplicates(sym, smap);
}

void SemanticAnaliser::checkDuplicates(std::string sym, std::map<std::string, Symbol>& symbols)
{
    auto found = symbols.find(sym);
    if(found != symbols.end()) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, found->second.line, found->second.pos));  
}
*/
//void SemanticAnaliser::checkDeclaration(std::string sym, std::vector<std::map<std::string, Symbol>>& symbols);
//void SemanticAnaliser::checkDeclaration(std::string sym, std::map<std::string, Symbol>& symbols);

std::map<std::string, Var>::iterator SemanticAnaliser::getVar(std::string sym, std::string class_name)
{
    std::map<std::string, Var>::iterator found;

    for(std::map<std::string,Var> &var_map : scope_stack)
        if((found = var_map.find(sym)) != var_map.end()) return found;

    if(class_name != std::string())
    {
        Class& c = getClass(class_name)->second;
        if((found = c.class_vars.find(sym)) != c.class_vars.end()) return found;

        if(class_name == private_access)
            if((found = c.private_vars.find(sym)) != c.private_vars.end()) return found;
    }

    if((found = global_vars.find(sym)) != global_vars.end()) return found;
    
    error_manager.handleError(Error(Error::Type::Uninitialized_variable, 0, 0));   //TODO

    return found;
}

std::map<std::string, Func>::iterator SemanticAnaliser::getFunc(std::string sym, std::string class_name)
{
    std::map<std::string, Func>::iterator found;

    if(class_name != std::string())
    {
        Class& c = getClass(class_name)->second;
        if((found = c.class_funcs.find(sym)) != c.class_funcs.end()) return found;

        if(class_name == private_access)
            if((found = c.private_funcs.find(sym)) != c.private_funcs.end()) return found;
    }

    if((found = global_funcs.find(sym)) != global_funcs.end()) return found;
    
    error_manager.handleError(Error(Error::Type::Uninitialized_function, 0, 0));   //TODO

    return found;
}

std::map<std::string, Class>::iterator SemanticAnaliser::getClass(std::string class_name)
{
    std::map<std::string, Class>::iterator found;

    if((found = classes.find(class_name)) != classes.end()) return found;

    error_manager.handleError(Error(Error::Type::Uninitialized_class, 0, 0));   //TODO

    return found;
}

std::map<std::string, Var>::iterator SemanticAnaliser::declareVar(ast::Node &root, bool priv)
{
    for(ast::Node &n : root.children)
    {
        Var new_var;
        new_var.name = n.production.value;
        new_var.type = root.production.value;
        new_var.pos = n.production.pos;
        new_var.line = n.production.row;

        getClass(new_var.type);

        if(scope_stack.size() == 0)
        {     
            if(private_access != std::string())
            {           
                Class& c = getClass(private_access)->second;
                checkDuplicates(new_var.name, c.class_vars);
                checkDuplicates(new_var.name, c.private_vars);

                if(!priv) c.class_vars[new_var.name] = new_var; //std::move?
                else c.private_vars[new_var.name] = new_var;
            }
            else
            {
                checkDuplicates(new_var.name, global_vars);
                global_vars[new_var.name] = new_var;
            }
        }
        else 
        {
            checkDuplicates(new_var.name, scope_stack);
            scope_stack.back()[new_var.name] = new_var;
        }
    }
}

std::map<std::string, Func>::iterator SemanticAnaliser::declareFunc(ast::Node &root, bool priv)
{
    Func new_func;
    new_func.type = root.production.value;
    new_func.pos = root.production.pos;
    new_func.line = root.production.row;

    size_t i;
    if(root.production.name == "Constructor declaration")
    {
        if(private_access != new_func.type)
            error_manager.handleError(Error(Error::Type::Bad_constructor, root.production.row, root.production.pos));
        
        new_func.name = std::string();

        i = 0;
    }
    else
    {
        getClass(new_func.type);

        new_func.name = root.children[0].production.value;
        i = 1;
    }
    
    while(i < root.children.size() && root.children[i].production.name == "Argument")
    {
        getClass(root.children[i].production.value);
        new_func.arg_types.push_back(root.children[i++].production.value);
    }

    new_func.name = new_func.getPrototype();

    if(private_access != std::string()) //what if it is constructor?
    {           
        Class& c = getClass(private_access)->second;
        checkDuplicates(new_func.name, c.class_funcs);
        checkDuplicates(new_func.name, c.private_funcs);

        if(!priv) c.class_funcs[new_func.name] = new_func; //std::move?
        else c.private_funcs[new_func.name] = new_func;
    }
    else
    {
        checkDuplicates(new_func.name, global_funcs);
        global_funcs[new_func.name] = new_func;
    }

    //and then statements
    checkSemantics(root);
}

void SemanticAnaliser::checkSemantics(ast::Node &root)
{
    openScope();
    //arguments
    size_t i = 0;
    while(i < root.children.size() && root.children[i].production.name == "Argument")
        declareVar(root.children[i++]);

    while(i < root.children.size())
    {
        ast::Node &n = root.children[i];

        if(n.production.name == "Variable declaration") 
        {
            declareVar(n);
        }
        else if(n.production.name == "If statement" || n.production.name == "While statement")
        {
            //n.printTree();
            checkType(n.children[0], "bool");
            
            for(size_t j=1; j<n.children.size(); ++j)
                checkSemantics(n.children[j]);
        }
        else if(n.production.name == "Statement")
        {
            checkSemantics(n);
        }
        else if(n.production.name == "Return statement")
        {
            std::string unitype = checkTypeUniformity(n);
            if(root.production.value == "void" && unitype != std::string()) 
                error_manager.handleError(Error(Error::Type::Bad_return, n.production.row, n.production.pos));
            else if(root.production.value != unitype)
                error_manager.handleError(Error(Error::Type::Bad_return, n.production.row, n.production.pos));
        }
        else
        {
            checkTypeUniformity(n);
        }

        ++i;
    }
    //block_st check
    //  if          (is cond bool type, then procced with statements)
    //  while       (is cond bool type, then procced with statements)
    //  assignment  (decl check, type check)
    //  return      (decl check, is returned type the same as func type)
    //  s_variable  (varDecl)
    //  anything else -> s_expr (decl check, type check)

    //   checkStatement:
    //      assignment & return & s_expr => typeUniformity
    //      s_variable => varDecl
    //      if & while => first_child -> typeUniformity == "bool", other children -> checkStatement
    closeScope();
}

std::string SemanticAnaliser::checkTypeUniformity(ast::Node &root)
{
    std::string type = std::string();

    //std::cout << "Prod: " << root.production.name << std::endl; //TODO

    if(root.production.name == "Function") return checkFunction(root);
    if(root.production.name == "Complex identifier") return checkVar(root);
    if(root.production.name == "Int") return "int";
    if(root.production.name == "Bool") return "bool";
    if(root.production.name == "Negation") return "bool";
    if(root.production.name == "Double") return "double";

    if(root.children.size() > 0)
    {
        type = checkTypeUniformity(root.children[0]);

        for(size_t j=1; j<root.children.size(); ++j) if(checkTypeUniformity(root.children[j]) != type) 
            error_manager.handleError(Error(Error::Type::Bad_type, root.children[j].production.row, root.children[j].production.pos));
    }
    
    if(root.production.name == "Relational expression") type = "bool";

    return type;  //void
}

std::string SemanticAnaliser::checkType(ast::Node &root, std::string type_name)
{
    std::string ret;
    if((ret = checkTypeUniformity(root)) != type_name) 
        error_manager.handleError(Error(Error::Type::Bad_type, root.production.row, root.production.row));  

    return ret;
}

std::string SemanticAnaliser::checkFunction(ast::Node &root)    //cid func TODO
{
    Func used_func;
    if(root.children[0].children.size() == 0)
        used_func.name = root.children[0].production.value;
    else
        used_func.name = root.children[0].children.back().production.value;

    used_func.line = root.production.row;
    used_func.pos = root.production.pos;

    for(size_t i=1; i<root.children.size(); ++i)
        used_func.arg_types.push_back(checkTypeUniformity(root.children[i]));

    used_func.name = used_func.getPrototype();

    return getFunc(used_func.name, private_access)->second.type;    //TODO its not private access
}

std::string SemanticAnaliser::checkVar(ast::Node &root)
{
    std::string name = root.production.value;
    std::string next_class = private_access;
    auto v = getVar(name, next_class);
    next_class = getClass(v->second.type)->second.name;

    for(ast::Node &n : root.children)
    {
        name = n.production.value;
        v = getVar(name, next_class);
        next_class = getClass(v->second.type)->second.name;
    }

    return next_class;
}

std::map<std::string, Class>::iterator SemanticAnaliser::declareClass(ast::Node &root)
{
    Class new_class;
    new_class.name = root.production.value;
    new_class.line = root.production.row;
    new_class.pos = root.production.pos;

    checkDuplicates(new_class.name, classes);
    private_access = new_class.name;
    classes[new_class.name] = new_class;

    for(ast::Node& n : root.children)
    {
        bool privacy = (n.production.name == "Private");

        for(ast::Node& j : n.children)
        {
            if(j.production.name == "Variable declaration") declareVar(j, privacy);
            else if(j.production.name == "Function declaration") declareFunc(j, privacy);
            else if(j.production.name == "Constructor declaration") declareFunc(j, privacy);
        }
    }

    private_access = std::string();
}

void SemanticAnaliser::analyse()
{
    ast::Node root = std::move(parser.parse());

    root.printTree();

    for(ast::Node &n : root.children) 
    {
        if(n.production.name == "Class") declareClass(n);
        if(n.production.name == "Function declaration") declareFunc(n);
        if(n.production.name == "Variable declaration") declareVar(n);
    }

    std::cout << "Global vars: " << std::endl;
    for(auto& i : global_vars) std::cout << '\t' << i.second.type << " " << i.second.name << std::endl;
std::cout << std::endl;
    std::cout << "Global Funcs: " << std::endl;
    for(auto& i : global_funcs) std::cout << '\t' << i.second.type << " " << i.second.name << std::endl;
std::cout << std::endl;
std::cout << std::endl;
    for(auto& c : classes)
    {
        if(c.second.name == "int" || c.second.name == "double" || c.second.name == "bool" || c.second.name == "void") continue;

        std::cout << "Class: " << c.second.name << std::endl;
std::cout << std::endl;
        std::cout << "Public funcs: " << std::endl;
        for(auto& i : c.second.class_funcs) std::cout << '\t' << i.second.type << " " << i.second.name << std::endl;
std::cout << std::endl;
        std::cout << "Private funcs: " << std::endl;
        for(auto& i : c.second.private_funcs) std::cout << '\t' << i.second.type << " " << i.second.name << std::endl;
std::cout << std::endl;
        std::cout << "Public vars: " << std::endl;
        for(auto& i : c.second.class_vars) std::cout << '\t' << i.second.type << " " << i.second.name << std::endl;
std::cout << std::endl;
        std::cout << "Private vars: " << std::endl;
        for(auto& i : c.second.private_vars) std::cout << '\t' << i.second.type << " " << i.second.name << std::endl;
std::cout << std::endl;
std::cout << std::endl;

        std::vector<std::string> war = error_manager.getWarnings();
        for(std::string s : war)
            std::cout << s << std::endl;
    }
}



/*
bool SemanticAnaliser::isVarDeclared(std::string variable)
{
    for(const Scope& s : scope) if(isVarDeclared(variable, s)) return true;

    return false;
}

bool SemanticAnaliser::isVarDeclared(std::string variable, const Scope& s)
{
    return s.sym_table.find(variable) != s.sym_table.end();
}

bool SemanticAnaliser::isFuncDeclared(std::string func_proto)
{
    for(const Scope& s : scope) if(isFuncDeclared(func_proto, s)) return true;

    return false;
}

bool SemanticAnaliser::isFuncDeclared(std::string func_proto, const Scope& s)
{
    return s.func_table.find(func_proto) != s.func_table.end();
}

bool SemanticAnaliser::isClassDeclared(std::string class_name)
{
    return type_table.find(class_name) != type_table.end();
}


void Scope::insert(Var&& v)
{
    sym_table.insert(std::move(v));
}

void Scope::insert(Func&& f)
{
    func_table.insert(std::move(f));
}

void SemanticAnaliser::openScope()
{
    scope.push_back(Scope());
}

void SemanticAnaliser::closeScope()
{
    for(auto v : scope.back().sym_table)
    if(v.usage_count == 0) 
        error_manager.handleError(Error(Error::Type::Unused_variable, v.line, v.pos));

    for(auto f : scope.back().func_table)
    if(f.usage_count == 0) 
        error_manager.handleError(Error(Error::Type::Unused_function, f.line, f.pos));

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

bool SemanticAnaliser::isClassDeclared(std::string class_name)
{
    return type_table.find(class_name) != type_table.end();
}

Func& SemanticAnaliser::getFunc(std::string func_proto)
{
    return *scope.back().func_table.find(func_proto);
}

void SemanticAnaliser::checkForClass(std::string name, int line, int pos)
{
    if(!isClassDeclared(name)) 
        error_manager.handleError(Error(Error::Type::Uninitialized_class, line, pos));
}

void SemanticAnaliser::checkForClass(ast::Node& root, bool should_exist)
{
    if(isClassDeclared(root.production.value) != should_exist) 
    {
        if(should_exist) error_manager.handleError(Error(Error::Type::Uninitialized_class, root.production.row, root.production.pos));
        else error_manager.handleError(Error(Error::Type::Multi_initialization, root.production.row, root.production.pos));
    }
}

void SemanticAnaliser::checkForVariable(ast::Node &root, bool should_exist)
{
    if(isVarDeclared(root.production.value) != should_exist)
    {
        if(should_exist) error_manager.handleError(Error(Error::Type::Uninitialized_variable, root.production.row, root.production.pos));
        else error_manager.handleError(Error(Error::Type::Multi_initialization, root.production.row, root.production.pos));
    }
}

void SemanticAnaliser::checkForFunction(ast::Node &root, bool should_exist)   //ain't good -> should have used func_proto?
{
    if(isFuncDeclared(root.production.value) != should_exist)
    {
        if(should_exist) error_manager.handleError(Error(Error::Type::Uninitialized_function, root.production.row, root.production.pos));   
        else error_manager.handleError(Error(Error::Type::Multi_initialization, root.production.row, root.production.pos));
    } 
}

std::string SemanticAnaliser::newFunc(ast::Node& root)  //root better be constructor(!) or function declaration
{
    checkForClass(root, true);

    Func new_func;
    new_func.name = root.children[0].production.value;
    new_func.type = root.production.value;//TODO type check
    new_func.pos = root.production.pos;
    new_func.line = root.production.row;

    size_t i = 0;
    while(i < root.children.size() && root.children[i].production.name == "Argument")
    {
        checkForClass(root.children[i], true);
        new_func.arg_types.push_back(root.children[i++].production.value);//TODO type check
    }
    
    if(isFuncDeclared(new_func.getPrototype())) //check for func
        error_manager.handleError(Error(Error::Type::Multi_initialization, root.production.row, root.production.pos));

    std::string ret = new_func.getPrototype();
    scope.back().insert(std::move(new_func));

    return ret;
}

std::vector<std::string> SemanticAnaliser::newVar(ast::Node& root)    //root better be argument or variable declaration
{
    checkForClass(root, true);
    std::vector<std::string> var_names; 

    for(ast::Node &n : root.children)
    {
        Var new_var;
        new_var.name = n.production.value;
        new_var.type = root.production.value;
        new_var.pos = n.production.pos;
        new_var.line = n.production.row;

        checkForVariable(n, false);

        var_names.push_back(new_var.name);
        scope.back().insert(std::move(new_var));
    }

    return var_names;
}

Class& SemanticAnaliser::newClass(ast::Node& root)
{
    checkForClass(root, false);
}

void SemanticAnaliser::checkSemantics(ast::Node &root)
{
    
}

void SemanticAnaliser::declareFunction(ast::Node &root)
{
    std::string f_name = newFunc(root);
    Func f = *scope.back().func_table.find(f_name);  //bad
    scope.back().func_table.find(f_name)->incUsage();

    openScope();

    for(size_t i=0; i<f.arg_types.size(); ++i)
        newVar(root.children[i+1]);

    for(size_t i=f.arg_types.size(); i<root.children.size(); ++i)
        checkSemantics(root.children[i]);

    closeScope();
}

void SemanticAnaliser::declareVariable(ast::Node &root)
{
    newVar(root);
}

void SemanticAnaliser::declareClass(ast::Node &root)
{
    Class& c = newClass(root);

    openScope();//open class scope?

    closeScope();
}
*/