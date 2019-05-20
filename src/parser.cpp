#include "parser.hpp"
#include <iostream>

using ast::Node;

ast::Node Parser::parse()
{
    ast::Node root("Root", 0, 0);

    program(root);

    return root;
}

Parser::Parser(Lexer &l, ErrorManager &em): lexer(l), error_manager(em)
{}

Token Parser::nextToken()
{
    Token ret;
    tokens.push_back(ret = lexer.getToken());
    return ret;
}

Token Parser::peekToken()
{
    if(tokens.empty()) return nextToken();

    return tokens.front();
}

bool Parser::expectToken(Token::Type type, size_t position)
{
    while(position >= tokens.size()) nextToken();

    return (tokens[position].getType() == type);
}

bool Parser::expectToken(std::set<Token::Type> types, size_t position)
{
    while(position >= tokens.size()) nextToken();

    return (types.find(tokens[position].getType()) != types.end());
}

Token Parser::acceptToken()
{
    Token t = peekToken();
    tokens.pop_front();

    return t;     
}

Token Parser::acceptToken(Token::Type type)
{
    Token t = peekToken();
std::cout << "DEBUG: " << '\t' << t.toString() << '\t' << Token(type,0,0).toString() << std::endl;
    if(t.getType() == type) tokens.pop_front();
    else error_manager.handleError(Error(Error::Type::Unexpected_token, t.getLine(), t.getPosition()));

    return t;    
}

Token Parser::acceptToken(std::set<Token::Type> types)
{
    Token t = peekToken();
std::cout << "DEBUG: " << '\t' << t.toString() << std::endl;
    if(expectToken(types,0)) tokens.pop_front();
    else error_manager.handleError(Error(Error::Type::Unexpected_token, t.getLine(), t.getPosition()));

    return t;    
}

void Parser::signalError()
{
    Token t = peekToken();
    error_manager.handleError(Error(Error::Type::Unexpected_token, t.getLine(), t.getPosition()));
}

void Parser::c_ident(ast::Node& n)  //OK
{
    Token start = acceptToken(Token::Type::Ident);
    ast::Node& cident_node = n.attachNode("Complex identifier", start.getLine(), start.getPosition(), start.toStringExtra());

    while(expectToken(Token::Type::Dot, 0))
    {
        acceptToken(Token::Type::Dot);
        Token ident = acceptToken(Token::Type::Ident);
        cident_node.attachNode("Identifier", ident.getLine(), ident.getPosition(), ident.getName());
    }
}

void Parser::s_variable(ast::Node& n)   //OK
{
    Token type = acceptToken(Token::Type::Ident);
    ast::Node& variable_node = n.attachNode("Variable declaration", type.getLine(), type.getPosition(), type.getName());

    Token name = acceptToken(Token::Type::Ident);
    variable_node.attachNode("Name", name.getLine(), name.getPosition(), name.getName());

    while(expectToken(Token::Type::Comma, 0))
    {
        acceptToken(Token::Type::Comma);
        name = acceptToken(Token::Type::Ident);
        variable_node.attachNode("Name", name.getLine(), name.getPosition(), name.getName());
    }

    acceptToken(Token::Type::Semicolon);
}

void Parser::s_function(ast::Node& n)   //OK
{
    Token f_type = acceptToken(Token::Type::Ident);
    ast::Node& function_node = n.attachNode("Function declaration", f_type.getLine(), f_type.getPosition(), f_type.getName());

    Token name = acceptToken(Token::Type::Ident);
    function_node.attachNode("Name", name.getLine(), name.getPosition(), name.getName());
    acceptToken(Token::Type::OpenBracket);

    if(expectToken(Token::Type::Ident, 0))
    {
        Token type = acceptToken(Token::Type::Ident);
        Token arg_name = acceptToken(Token::Type::Ident);

        ast::Node& argument = function_node.attachNode("Argument", type.getLine(), type.getPosition(), type.getName());
        argument.attachNode("Name", arg_name.getLine(), arg_name.getPosition(), arg_name.getName());

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            
            type = acceptToken(Token::Type::Ident);
            arg_name = acceptToken(Token::Type::Ident);

            ast::Node& argument = function_node.attachNode("Argument", type.getLine(), type.getPosition(), type.getName());
            argument.attachNode("Name", arg_name.getLine(), arg_name.getPosition(), arg_name.getName());
        }
    }

    acceptToken(Token::Type::CloseBracket);
    block_st(function_node);
}

void Parser::assignment(ast::Node& n)   //OK
{
    Token start = peekToken();
    ast::Node& assign_node = n.attachNode("Assignment", start.getLine(), start.getPosition());

    c_ident(assign_node);

    acceptToken(Token::Type::Assignment);

    expr(assign_node);

    acceptToken(Token::Type::Semicolon);

}

void Parser::return_st(ast::Node& n)    //OK
{
    Token start = acceptToken(Token::Type::Return);
    ast::Node& return_node = n.attachNode("Return statement", start.getLine(), start.getPosition());

    if(!expectToken(Token::Type::Semicolon, 0)) expr(return_node); //optional

    acceptToken(Token::Type::Semicolon);
}

void Parser::statement(ast::Node& n)    //OK
{
    Token start = peekToken();
    ast::Node& st_node = n.attachNode("Statement", start.getLine(), start.getPosition());

    if(expectToken(Token::Type::OpenCurly, 0)) block_st(st_node);
    else simple_st(st_node);
}

void Parser::while_st(ast::Node& n) //OK
{
    Token start = acceptToken(Token::Type::While);
    ast::Node& while_node = n.attachNode("While statement", start.getLine(), start.getPosition());

    acceptToken(Token::Type::OpenBracket);
    bool_expr(while_node);
    acceptToken(Token::Type::CloseBracket);

    statement(while_node);
}

void Parser::if_st(ast::Node& n)    //OK
{
    Token start = acceptToken(Token::Type::If);
    ast::Node& if_node = n.attachNode("If statement", start.getLine(), start.getPosition());
    acceptToken(Token::Type::OpenBracket);
    bool_expr(if_node);
    acceptToken(Token::Type::CloseBracket);

    statement(if_node);

    if(expectToken(Token::Type::Else, 0))
    {
        acceptToken(Token::Type::Else);
        statement(if_node);
    }
}

void Parser::function(ast::Node& n) //OK
{
    Token start = peekToken();
    ast::Node& function_node = n.attachNode("Function", start.getLine(), start.getPosition());
    c_ident(function_node);
    acceptToken(Token::Type::OpenBracket);

    if(!expectToken(Token::Type::CloseBracket, 0))
    {
        expr(function_node);

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            expr(function_node);
        }
    }

    acceptToken(Token::Type::CloseBracket);
}

void Parser::comp_expr(ast::Node& n, ast::Node backtracking)    //OK
{
    Token start = peekToken();
    ast::Node& cexpr_node = n.attachNode("Relational expression", start.getLine(), start.getPosition());
    simple_expr(cexpr_node, backtracking);

    Token oper = acceptToken(rel_operators);
    cexpr_node.production.value = oper.toString();

    simple_expr(cexpr_node);
}

void Parser::expr(ast::Node& n) //TBD
{
    if(expectToken(Token::Type::Bool)) bool_expr(n);
    else simple_expr(n);
}

void Parser::bool_ele(ast::Node& n) //TBD
{
    Token start = peekToken();
    ast::Node& bool_node = n.attachNode("Bool element", start.getLine(), start.getPosition());

    if(expectToken(Token::Type::Not,0)) 
    {
        Token neg = acceptToken(Token::Type::Not);
        bool_node.attachNode("Negation", neg.getLine(), neg.getPosition());
    }

    if(expectToken(Token::Type::Bool,0)) 
    {
        Token ele = acceptToken(Token::Type::Bool);
        bool_node.attachNode(ele.toString(), ele.getLine(), ele.getPosition(), ele.toStringExtra());
    }
    else if(expectToken(const_arithm) || expectToken(sign)) comp_expr(bool_node);
    else if(expectToken(Token::Type::Ident))
    {
        ast::Node temp("Temp",0,0);

        int i = 0;
        while(expectToken(Token::Type::Ident, i++)) //TODO
        {
            if(expectToken(Token::Type::OpenBracket, i)) function(temp);
            else if(!expectToken(Token::Type::Dot, i++)) c_ident(temp);
            else continue;

            if(expectToken(add_operators) || expectToken(mul_operators) || expectToken(rel_operators)) 
                comp_expr(bool_node, std::move(temp.children.front()));
            else 
                bool_node.attachNode(std::move(temp.children.front()));

            return;
        }
        //error
    }
    else signalError();
}

void Parser::bool_expr2(ast::Node& n)   //OK
{
    Token start = peekToken();
    ast::Node expr_node("Bool expression", start.getLine(), start.getPosition());

    bool_ele(expr_node);

    expr_node = std::move(expr_node.children.front());

    while(expectToken(Token::Type::And))
    {
        Token oper = acceptToken(Token::Type::And);

        ast::Node temp(oper.toString(), oper.getLine(), oper.getPosition());
        temp.attachNode(std::move(expr_node));

        bool_ele(temp);
        expr_node = std::move(temp);
    }

    n.attachNode(std::move(expr_node));
}

void Parser::bool_expr(ast::Node& n)    //OK
{
    Token start = peekToken();
    ast::Node expr_node("Bool expression", start.getLine(), start.getPosition());

    bool_expr2(expr_node);

    expr_node = std::move(expr_node.children.front());

    while(expectToken(Token::Type::Or))
    {
        Token oper = acceptToken(Token::Type::Or);

        ast::Node temp(oper.toString(), oper.getLine(), oper.getPosition());
        temp.attachNode(std::move(expr_node));

        bool_expr2(temp);
        expr_node = std::move(temp);
    }

    n.attachNode(std::move(expr_node));
}

void Parser::arithm_ele(ast::Node& n, ast::Node backtracking)   //OK (w/o unary sign)
{
    if(!backtracking.isEmpty())
    {
        if((backtracking.production.name == "Complex identifier" || backtracking.production.name == "Function")) 
            n.attachNode(std::move(backtracking));
        else signalError();

        return;
    }

    //Token start = peekToken();
    //ast::Node& arithm_node = n.attachNode("Arithm element", start.getLine(), start.getPosition());

    //if(expectToken(sign,0)) acceptToken(sign);

    if(expectToken(const_arithm, 0))
    {
        Token ele = acceptToken(const_arithm);
        n.attachNode(ele.toString(), ele.getLine(), ele.getPosition(), ele.toStringExtra());
    } 
    else if(expectToken(Token::Type::Ident))
    {
        int i = 0;
        while(expectToken(Token::Type::Ident, i++))//TODO
        {
            if(expectToken(Token::Type::OpenBracket, i)) function(n);
            else if(!expectToken(Token::Type::Dot, i++)) c_ident(n);
            else continue;

            return;
        }
    }
    else signalError(); //TODO
}

void Parser::simple_expr2(ast::Node& n, ast::Node backtracking) //OK
{
    Token start = peekToken();
    ast::Node expr_node("Simple expression", start.getLine(), start.getPosition());

    arithm_ele(expr_node, backtracking);

    expr_node = std::move(expr_node.children.front());

    while(expectToken(mul_operators))
    {
        Token oper = acceptToken(mul_operators);

        ast::Node temp(oper.toString(), oper.getLine(), oper.getPosition());
        temp.attachNode(std::move(expr_node));

        arithm_ele(temp);
        expr_node = std::move(temp);
    }

    n.attachNode(std::move(expr_node));
}

void Parser::simple_expr(ast::Node& n, ast::Node backtracking)  //OK
{
    Token start = peekToken();
    ast::Node expr_node("Simple expression", start.getLine(), start.getPosition());

    simple_expr2(expr_node, backtracking);

    expr_node = std::move(expr_node.children.front());

    while(expectToken(add_operators))
    {
        Token oper = acceptToken(add_operators);

        ast::Node temp(oper.toString(), oper.getLine(), oper.getPosition());
        temp.attachNode(std::move(expr_node));

        simple_expr2(temp);
        expr_node = std::move(temp);
    }

    n.attachNode(std::move(expr_node));
}

void Parser::s_simple_expr(ast::Node& n)    //OK
{
    simple_expr(n);
    acceptToken(Token::Type::Semicolon);
}

void Parser::simple_st(ast::Node& n)    //OK
{
    if(expectToken(Token::Type::If, 0)) if_st(n);
    else if(expectToken(Token::Type::While, 0)) while_st(n);
    else if(expectToken(Token::Type::Return, 0)) return_st(n);
    else if(expectToken(Token::Type::Ident, 0) && expectToken(Token::Type::Ident, 1)) s_variable(n);
    else if(expectToken(Token::Type::Minus, 0) || expectToken(Token::Type::Plus, 0) ||
            expectToken(Token::Type::Int, 0) || expectToken(Token::Type::Double, 0)) s_simple_expr(n);
    else if(expectToken(Token::Type::Ident))
    {
        int i = 0;
        while(expectToken(Token::Type::Ident, i++))
        {
            if(expectToken(Token::Type::Assignment, i)) assignment(n);
            //else if(expectToken(Token::Type::OpenBracket, i)) s_simple_expr(n);
            else if(!expectToken(Token::Type::Dot, i++)) s_simple_expr(n);
            else continue;

            return;
        }
    }
    else signalError();
}

void Parser::block_st(ast::Node& n) //OK
{
    acceptToken(Token::Type::OpenCurly);

    while(!expectToken(Token::Type::CloseCurly, 0)) simple_st(n);   //TBD

    acceptToken(Token::Type::CloseCurly);
}

void Parser::s_constructor(ast::Node& n)    //OK
{
    Token start = acceptToken(Token::Type::Ident);
    ast::Node& constructor_node = n.attachNode("Constructor declaration", start.getLine(), start.getPosition(), start.getName());
    acceptToken(Token::Type::OpenBracket);

    if(expectToken(Token::Type::Ident, 0))
    {
        Token type = acceptToken(Token::Type::Ident);
        Token arg_name = acceptToken(Token::Type::Ident);

        ast::Node& argument = constructor_node.attachNode("Argument", type.getLine(), type.getPosition(), type.getName());
        argument.attachNode("Name", arg_name.getLine(), arg_name.getPosition(), arg_name.getName());

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            
            type = acceptToken(Token::Type::Ident);
            arg_name = acceptToken(Token::Type::Ident);

            ast::Node& argument = constructor_node.attachNode("Argument", type.getLine(), type.getPosition(), type.getName());
            argument.attachNode("Name", arg_name.getLine(), arg_name.getPosition(), arg_name.getName());
        }
    }

    acceptToken(Token::Type::CloseBracket);
    block_st(constructor_node);
}

void Parser::private_part(ast::Node& n) //OK
{
    Token start = acceptToken(Token::Type::Private);
    acceptToken(Token::Type::Colon);

    ast::Node& private_node = n.attachNode("Private", start.getLine(), start.getPosition());

    while(true)
    {
        if(!expectToken(Token::Type::Ident, 0)) break;

        if(expectToken(Token::Type::OpenBracket, 1)) s_constructor(private_node);
        else if(expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) s_function(private_node);
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) s_variable(private_node);
            else break; 
        }
        else break; 
    }
}

void Parser::public_part(ast::Node& n)  //OK
{
    Token start = acceptToken(Token::Type::Public);
    acceptToken(Token::Type::Colon);

    ast::Node& public_node = n.attachNode("Public", start.getLine(), start.getPosition());

    while(true)
    {
        if(!expectToken(Token::Type::Ident, 0)) break;

        if(expectToken(Token::Type::OpenBracket, 1)) s_constructor(public_node);
        else if(expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) s_function(public_node);
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) s_variable(public_node);
            else break; 
        }
        else break; 
    }
}

void Parser::class_content(ast::Node& n)  //OK
{
    while(true)
    {
        if(expectToken(Token::Type::Private, 0)) private_part(n);
        else if(expectToken(Token::Type::Public, 0)) public_part(n);
        else break;
    }
}

void Parser::s_class(ast::Node& n)  //OK
{
    Token start = acceptToken(Token::Type::Class);
    Token name = acceptToken(Token::Type::Ident);
    ast::Node& class_node = n.attachNode("Class", start.getLine(), start.getPosition(), name.getName());

    acceptToken(Token::Type::OpenCurly);
    class_content(class_node);
    acceptToken(Token::Type::CloseCurly);
    acceptToken(Token::Type::Semicolon);
}

void Parser::program(ast::Node& n)  //OK
{
    while(true)
    {
        if(expectToken(Token::Type::Class, 0)) s_class(n);
        else if(expectToken(Token::Type::Ident, 0) && expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) s_function(n);
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) s_variable(n);
            else signalError(); 
        }
        else break;
    }

    if(peekToken().getType() != Token::Type::Eof) signalError();
}