#include "semantic.hpp"

void SemanticAnaliser::openScope()
{
    scope_stack.push_back(std::map<std::string,Var>());
}

void SemanticAnaliser::closeScope()
{
    for(const auto &v : scope_stack.back())
    if(v.second.usage_count == 0) 
        error_manager.handleError(Error(Error::Type::Unused_variable, v.second.line, v.second.pos, v.second.name));

    //pushInfo("Closed scope variables", "No variables in closed scope", scope_stack.back());
    
    scope_stack.pop_back();
}

void SemanticAnaliser::checkDuplicates(Symbol sym, std::vector<std::map<std::string, Var>>& symbols)
{
    for(std::map<std::string, Var>& smap : symbols)
        checkDuplicates(sym, smap);
}

void SemanticAnaliser::checkDuplicates(Symbol sym, std::map<std::string, Var>& symbols)    //triple repetition TODO
{
    auto found = symbols.find(sym.name);
    if(found != symbols.end()) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, sym.line, sym.pos, sym.name));  
}

void SemanticAnaliser::checkDuplicates(Symbol sym, std::map<std::string, Func>& symbols)
{
    auto found = symbols.find(sym.name);
    if(found != symbols.end()) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, sym.line, sym.pos, sym.name));  
}

void SemanticAnaliser::checkDuplicates(Symbol sym, std::map<std::string, Class>& symbols)
{
    auto found = symbols.find(sym.name);
    if(found != symbols.end()) 
        error_manager.handleError(Error(Error::Type::Multi_initialization, sym.line, sym.pos, sym.name));  
}

std::map<std::string, Var>::iterator SemanticAnaliser::getMemberVar(Symbol sym, std::string class_name)
{
    std::map<std::string, Var>::iterator found;

    Class& c = getClass(Symbol(class_name, sym.line, sym.pos))->second;
    if((found = c.class_vars.find(sym.name)) != c.class_vars.end())
    {
        found->second.usage_count++;
        return found;
    }

    if(class_name == private_access)
    if((found = c.private_vars.find(sym.name)) != c.private_vars.end())         
    {
        found->second.usage_count++;
        return found;
    }

    error_manager.handleError(Error(Error::Type::Uninitialized_variable, sym.line, sym.pos, sym.name));

    return found;
}

std::map<std::string, Var>::iterator SemanticAnaliser::getVar(Symbol sym, std::string class_name)
{
    std::map<std::string, Var>::iterator found;

    for(std::map<std::string,Var> &var_map : scope_stack)
    if((found = var_map.find(sym.name)) != var_map.end()) 
    {
        found->second.usage_count++;
        return found;
    }

    if(class_name != std::string())
    {
        Class& c = getClass(Symbol(class_name, sym.line, sym.pos))->second;
        if((found = c.class_vars.find(sym.name)) != c.class_vars.end())
        {
            found->second.usage_count++;
            return found;
        }

        if(class_name == private_access)
        if((found = c.private_vars.find(sym.name)) != c.private_vars.end())         
        {
            found->second.usage_count++;
            return found;
        }
    }

    if((found = global_vars.find(sym.name)) != global_vars.end())
    {
        found->second.usage_count++;
        return found;
    } 

    error_manager.handleError(Error(Error::Type::Uninitialized_variable, sym.line, sym.pos, sym.name));    //TODO

    return found;
}

std::map<std::string, Func>::iterator SemanticAnaliser::getFunc(Symbol sym, std::string class_name)
{
    std::map<std::string, Func>::iterator found;

    if(class_name != std::string())
    {
        Class& c = getClass(Symbol(class_name, sym.line, sym.pos))->second;
        if((found = c.class_funcs.find(sym.name)) != c.class_funcs.end()) 
        {
            found->second.usage_count++;
            return found;
        }

        if(class_name == private_access) 
        if((found = c.private_funcs.find(sym.name)) != c.private_funcs.end()) 
        {
            found->second.usage_count++;
            return found;
        }
    }
    
    if((found = global_funcs.find(sym.name)) != global_funcs.end()) 
    {
        found->second.usage_count++;
        return found;
    }
    
    error_manager.handleError(Error(Error::Type::Uninitialized_function, sym.line, sym.pos, sym.name));

    return found;
}

std::map<std::string, Class>::iterator SemanticAnaliser::getClass(Symbol class_name)
{
    std::map<std::string, Class>::iterator found;

    if((found = classes.find(class_name.name)) != classes.end()) 
    {
        if(private_access != found->second.name) found->second.usage_count++;
        return found;
    }

    error_manager.handleError(Error(Error::Type::Uninitialized_class, class_name.line, class_name.pos, class_name.name));

    return found;
}

void SemanticAnaliser::declareVar(ast::Node &root, bool priv)
{
    for(ast::Node &n : root.children)
    {
        Var new_var;
        new_var.name = n.production.value;
        new_var.type = root.production.value;
        new_var.pos = n.production.pos;
        new_var.line = n.production.row;

        getClass(Symbol(new_var.type, new_var.line, new_var.pos));

        if(scope_stack.size() == 0)
        {     
            if(private_access != std::string())
            {           
                Class& c = getClass(Symbol(private_access, new_var.line, new_var.pos))->second;
                checkDuplicates(new_var, c.class_vars);
                checkDuplicates(new_var, c.private_vars);

                if(!priv) c.class_vars[new_var.name] = new_var; //std::move?
                else c.private_vars[new_var.name] = new_var;
            }
            else
            {
                checkDuplicates(new_var, global_vars);
                global_vars[new_var.name] = new_var;
            }
        }
        else 
        {
            checkDuplicates(new_var, scope_stack);
            scope_stack.back()[new_var.name] = new_var;
        }
    }
}

void SemanticAnaliser::declareFunc(ast::Node &root, bool priv) //TODO CONSTRUCTORS
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
        function_return = std::string();

        i = 0;
    }
    else
    {
        getClass(Symbol(new_func.type, new_func.line, new_func.pos));

        new_func.name = root.children[0].production.value;
        if(new_func.type == "void") function_return = std::string();
        else function_return = new_func.type;

        i = 1;
    }
    
    while(i < root.children.size() && root.children[i].production.name == "Argument")
    {
        getClass(Symbol(root.children[i].production.value, root.children[i].production.row, root.children[i].production.pos));
        new_func.arg_types.push_back(root.children[i++].production.value);
    }

    new_func.name = new_func.getPrototype();

    if(private_access != std::string())
    {           
        Class& c = getClass(Symbol(private_access, new_func.line, new_func.pos))->second;
        checkDuplicates(new_func, c.class_funcs);
        checkDuplicates(new_func, c.private_funcs);

        if(!priv) c.class_funcs[new_func.name] = new_func;
        else c.private_funcs[new_func.name] = new_func;
    }
    else
    {
        checkDuplicates(new_func, global_funcs);
        global_funcs[new_func.name] = new_func;
    }

    checkSemantics(root);
}

void SemanticAnaliser::checkSemantics(ast::Node &root)
{
    openScope();
    //arguments
    size_t i = 1;
    if(root.production.name == "Constructor declaration") i = 0;    //TODO
    if(root.production.name == "Statement" || root.production.name == "If statement" || root.production.name == "While statement") i = 0;    //TODO

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
            checkType(n.children[0], "bool");
            
            for(size_t j=1; j<n.children.size(); ++j)
                checkSemantics(n.children[j]);                      
        }
        else if(n.production.name == "Return statement")    //TODO
        {
            std::string unitype = checkTypeUniformity(n);
            
            if(unitype != function_return)
                error_manager.handleError(Error(Error::Type::Bad_return, n.production.row, n.production.pos));
        }
        else
        {
            if(n.production.name != "Assignment" && n.production.name != "Function") 
                error_manager.handleError(Error(Error::Type::Unused_const, n.production.row, n.production.pos));
            checkTypeUniformity(n);
        }

        ++i;
    }

    closeScope();
}

std::string SemanticAnaliser::checkTypeUniformity(ast::Node &root)
{
    std::string type = std::string();

    if(root.production.name == "Function") 
    {
        if(root.children.front().children.size() == 0 && classes.find(root.children.front().production.value) != classes.end()) 
            return checkConstructor(root);  //TODO findDecl or something

        return checkFunction(root);
    }
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

std::string SemanticAnaliser::checkFunction(ast::Node &root)
{
    Func used_func;

    used_func.line = root.production.row;
    used_func.pos = root.production.pos;

    if(root.children[0].children.size() == 0)
        used_func.name = root.children[0].production.value;
    else
        used_func.name = root.children[0].children.back().production.value; //TODO?

    for(size_t i=1; i<root.children.size(); ++i)
        used_func.arg_types.push_back(checkTypeUniformity(root.children[i]));

    used_func.name = used_func.getPrototype();

    return getFunc(used_func, extractFuncClass(root))->second.type;
}

std::string SemanticAnaliser::checkConstructor(ast::Node &root)
{
    Func used_func;

    used_func.line = root.production.row;
    used_func.pos = root.production.pos;

    std::string class_name = root.children[0].production.value;

    for(size_t i=1; i<root.children.size(); ++i)
        used_func.arg_types.push_back(checkTypeUniformity(root.children[i]));

    used_func.name = used_func.getPrototype();

    return getFunc(used_func, class_name)->second.type;
}

std::string SemanticAnaliser::checkVar(ast::Node &root)
{
    std::string name = root.production.value;
    int line = root.production.row, pos = root.production.pos;

    std::string next_class = private_access;
    auto v = getVar(Symbol(name, line, pos), next_class);
    next_class = getClass(Symbol(v->second.type, line, pos))->second.name;

    for(ast::Node &n : root.children)
    {
        name = n.production.value;
        line = n.production.row, pos = n.production.pos;
        v = getMemberVar(Symbol(name, line, pos), next_class);
        next_class = getClass(Symbol(v->second.type, line, pos))->second.name;
    }

    return next_class;
}

std::string SemanticAnaliser::extractFuncClass(ast::Node &root) //TODO
{
    ast::Node &name_node = root.children[0];
    if(name_node.children.size() == 0) return private_access;

    std::string name = name_node.production.value;
    int line = name_node.production.row, pos = name_node.production.pos;

    std::string next_class = private_access;

    auto v = getVar(Symbol(name, line, pos), next_class);
    next_class = getClass(Symbol(v->second.type, line, pos))->second.name;

    for(size_t i=0; i+1<name_node.children.size(); i++)
    {
        name = name_node.children[i].production.value;
        line = name_node.children[i].production.row, pos = name_node.children[i].production.pos;
        v = getMemberVar(Symbol(name, line, pos), next_class);
        next_class = getClass(Symbol(v->second.type, line, pos))->second.name;
    }

    return next_class;
}

void SemanticAnaliser::declareClass(ast::Node &root)
{
    Class new_class;
    new_class.name = root.production.value;
    new_class.line = root.production.row;
    new_class.pos = root.production.pos;

    checkDuplicates(new_class, classes);
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

void SemanticAnaliser::produceWarnings()
{
    for(const auto &v : global_vars)
    if(v.second.usage_count == 0) 
        error_manager.handleError(Error(Error::Type::Unused_variable, v.second.line, v.second.pos, v.second.name));

    for(const auto &f : global_funcs)
    if(f.second.usage_count == 0) 
    {
        if(f.second.name == "main()") continue;
        error_manager.handleError(Error(Error::Type::Unused_function, f.second.line, f.second.pos, f.second.name));
    }
        
    for(const auto &c : classes)
    {
        if(c.second.name == "int" || c.second.name == "double" || c.second.name == "bool" || c.second.name == "void") continue;

        if(c.second.usage_count == 0)
            error_manager.handleError(Error(Error::Type::Unused_class, c.second.line, c.second.pos, c.second.name));

        for(const auto &v : c.second.class_vars)
        if(v.second.usage_count == 0) 
            error_manager.handleError(Error(Error::Type::Unused_variable, v.second.line, v.second.pos, c.second.name + "::" + v.second.name));

        for(const auto &v : c.second.private_vars)
        if(v.second.usage_count == 0) 
            error_manager.handleError(Error(Error::Type::Unused_variable, v.second.line, v.second.pos, c.second.name + "::" + v.second.name));

        for(const auto &f : c.second.class_funcs)
        if(f.second.usage_count == 0)
        {
            if(f.second.name.front() == '(')
                error_manager.handleError(Error(Error::Type::Unused_constructor, f.second.line, f.second.pos, c.second.name + f.second.name));
            else 
                error_manager.handleError(Error(Error::Type::Unused_function, f.second.line, f.second.pos, c.second.name + "::" + f.second.name));
        }

        for(const auto &f : c.second.private_funcs)
        if(f.second.usage_count == 0) 
        {
            if(f.second.name.front() == '(')
                error_manager.handleError(Error(Error::Type::Unused_constructor, f.second.line, f.second.pos, c.second.name + f.second.name));
            else 
                error_manager.handleError(Error(Error::Type::Unused_function, f.second.line, f.second.pos, c.second.name + "::" + f.second.name));
        }
    }    
}

void SemanticAnaliser::analyse()
{
    ast::Node root = std::move(parser.parse());

    for(ast::Node &n : root.children) 
    {
        if(n.production.name == "Class") declareClass(n);
        if(n.production.name == "Function declaration") declareFunc(n);
        if(n.production.name == "Variable declaration") declareVar(n);
    }

    collectSymbolsInfo();
    produceWarnings();
}

void SemanticAnaliser::pushInfo(std::string title, std::string empty_title, std::map<std::string, Var>& sym_map)
{
    std::vector<std::string> scope_info;
    if(sym_map.size() > 0)
    {
        scope_info.push_back(title);
        for(auto& i : sym_map) 
            scope_info.push_back("Usage: " + std::to_string(i.second.usage_count) + '\t' + i.second.type + " " + i.second.name);
    }
    else scope_info.push_back(empty_title);
    
    analise_info.push_back(std::move(scope_info));
}

void SemanticAnaliser::pushInfo(std::string title, std::string empty_title, std::map<std::string, Func>& sym_map)
{
    std::vector<std::string> scope_info;
    if(sym_map.size() > 0)
    {
        scope_info.push_back(title);
        for(auto& i : sym_map) 
            scope_info.push_back("Usage: " + std::to_string(i.second.usage_count) + '\t' + i.second.type + " " + i.second.name);
    }
    else scope_info.push_back(empty_title);
    
    analise_info.push_back(std::move(scope_info));
}

void SemanticAnaliser::pushInfo(std::string title, std::string empty_title, std::map<std::string, Class>& sym_map)
{
    std::vector<std::string> scope_info;
    if(sym_map.size() > 4)
    {
        scope_info.push_back(title);
        for(auto& c : sym_map)
        {
            if(c.second.name == "int" || c.second.name == "double" || c.second.name == "bool" || c.second.name == "void") continue;
            scope_info.push_back("Usage: " + std::to_string(c.second.usage_count) + '\t' + c.second.name);
        } 
    }
    else scope_info.push_back(empty_title);
    
    analise_info.push_back(std::move(scope_info));
}

void SemanticAnaliser::collectSymbolsInfo()
{
    pushInfo("Global variables", "No global variables", global_vars);
    pushInfo("Global functions", "No global functions", global_funcs);
    pushInfo("Classes", "No classes", classes);

    for(auto& c : classes)
    {
        if(c.second.name == "int" || c.second.name == "double" || c.second.name == "bool" || c.second.name == "void") continue;

        pushInfo("Public functions (" + c.second.name + ")", "No public functions (" + c.second.name + ")", c.second.class_funcs);
        pushInfo("Private functions (" + c.second.name + ")", "No private functions (" + c.second.name + ")", c.second.private_funcs);
        pushInfo("Public variables (" + c.second.name + ")", "No public functions (" + c.second.name + ")", c.second.class_vars);
        pushInfo("Private variables (" + c.second.name + ")", "No private variables(" + c.second.name + ")", c.second.private_vars);
    }
}

void SemanticAnaliser::printAnaliseInfo()
{
    for(std::vector<std::string>& ov : analise_info)
    {
        std::cout << ov.front() << std::endl;
        for(size_t i=1;i<ov.size();++i) std::cout << '\t' << ov[i] << std::endl;
        std::cout << std::endl;
    }
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