#include "headers/parser.hpp"
#include <iostream>
void Parser::parse()
{
    program();
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

void Parser::acceptToken()
{
    peekToken();
    tokens.pop_front();            
}

void Parser::acceptToken(Token::Type type)
{
    Token t = peekToken();
std::cout << "DEBUG: " << '\t' << t.toString() << '\t' << Token(type,0,0).toString() << std::endl;
    if(t.getType() == type) tokens.pop_front();
    else error_manager.handleError(Error(Error::Type::Unexpected_token, t.getLine(), t.getPosition()));
}

void Parser::acceptToken(std::set<Token::Type> types)
{
    Token t = peekToken();
std::cout << "DEBUG: " << '\t' << t.toString() << std::endl;
    if(expectToken(types,0)) tokens.pop_front();
    else error_manager.handleError(Error(Error::Type::Unexpected_token, t.getLine(), t.getPosition()));
}

void Parser::c_ident()
{
    acceptToken(Token::Type::Ident);

    while(expectToken(Token::Type::Dot, 0))
    {
        acceptToken(Token::Type::Dot);
        acceptToken(Token::Type::Ident);
    }
}

void Parser::s_variable()
{
    acceptToken(Token::Type::Ident);
    acceptToken(Token::Type::Ident);

    while(expectToken(Token::Type::Comma, 0))
    {
        acceptToken(Token::Type::Comma);
        acceptToken(Token::Type::Ident);
    }

    acceptToken(Token::Type::Semicolon);
}

void Parser::s_function()
{
    acceptToken(Token::Type::Ident);
    acceptToken(Token::Type::Ident);
    acceptToken(Token::Type::OpenBracket);

    if(expectToken(Token::Type::Ident, 0))
    {
        acceptToken(Token::Type::Ident);
        acceptToken(Token::Type::Ident);

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            acceptToken(Token::Type::Ident);
            acceptToken(Token::Type::Ident);
        }
    }

    acceptToken(Token::Type::CloseBracket);
    block_st();
}

void Parser::comp_expr()
{
    simple_expr();

    acceptToken(rel_operators);

    simple_expr();
}

void Parser::expr()
{
    std::cout << "TODO1" << std::endl;
    simple_expr();
    std::cout << "TODO2" << std::endl;
}

void Parser::assignment()
{
    c_ident();

    acceptToken(Token::Type::Assignment);

    expr();

    acceptToken(Token::Type::Semicolon);

}

void Parser::return_st()
{
    acceptToken(Token::Type::Return);

    expr(); //optional

    acceptToken(Token::Type::Semicolon);
}

void Parser::statement()
{
    if(expectToken(Token::Type::OpenCurly, 0)) block_st();
    simple_st();
}

void Parser::while_st()
{
    acceptToken(Token::Type::While);
    acceptToken(Token::Type::OpenBracket);
    bool_expr();
    acceptToken(Token::Type::CloseBracket);

    statement();
}

void Parser::if_st()
{
    acceptToken(Token::Type::If);
    acceptToken(Token::Type::OpenBracket);
    bool_expr();
    acceptToken(Token::Type::CloseBracket);

    statement();

    if(expectToken(Token::Type::Else, 0))
    {
        acceptToken(Token::Type::Else);
        statement();
    }
}

void Parser::function()
{
    c_ident();
    acceptToken(Token::Type::OpenBracket);
/*
    if(expectToken(Token::Type::Ident, 0))
    {
        c_ident();

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            c_ident();
        }
    }*/
    while(!expectToken(Token::Type::CloseBracket, 0))
    {
        expr();

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            expr();
        }
    }

    acceptToken(Token::Type::CloseBracket);
}

void Parser::bool_ele() //TBD
{
    if(expectToken(Token::Type::Not,0)) acceptToken(Token::Type::Not);

    if(expectToken(Token::Type::Bool, 0)) acceptToken(Token::Type::Bool);
    else
    {
        comp_expr();

        /*int i = 0;
        while(expectToken(Token::Type::Ident, i++)) //TODO
        {
            if(expectToken(Token::Type::OpenBracket, i)) function();
            else if(!expectToken(Token::Type::Dot, i++)) c_ident();
            else continue;
            break;
        }*/
    }
}

void Parser::bool_expr2()
{
    bool_ele();

    while(expectToken(Token::Type::And, 0))
    {
        acceptToken(Token::Type::And);
        bool_ele();
    }
}

void Parser::bool_expr()
{
    bool_expr2();

    while(expectToken(Token::Type::Or, 0))
    {
        acceptToken(Token::Type::Or);
        bool_expr2();
    }
}

void Parser::arithm_ele()
{
    if(expectToken(sign,0)) acceptToken(sign);

    if(expectToken(const_arithm, 0)) acceptToken(const_arithm);
    else
    {
        int i = 0;
        while(expectToken(Token::Type::Ident, i++))//TODO
        {
            if(expectToken(Token::Type::OpenBracket, i)) function();
            else if(!expectToken(Token::Type::Dot, i++)) c_ident();
            else continue;
            break;
        }
    }
}

void Parser::simple_expr2()
{
    arithm_ele();

    while(expectToken(mul_operators, 0))
    {
        acceptToken(mul_operators);
        arithm_ele();
    }
}

void Parser::simple_expr()
{
    simple_expr2();

    while(expectToken(add_operators, 0))
    {
        acceptToken(add_operators);
        simple_expr2();
    }
}

void Parser::s_simple_expr()
{
    simple_expr();
    acceptToken(Token::Type::Semicolon);
}

void Parser::simple_st()
{
    if(expectToken(Token::Type::If, 0)) if_st();
    else if(expectToken(Token::Type::While, 0)) while_st();
    else if(expectToken(Token::Type::Return, 0)) return_st();
    else if(expectToken(Token::Type::Ident, 0) && expectToken(Token::Type::Ident, 1)) s_variable();
    else if(expectToken(Token::Type::Minus, 0) || expectToken(Token::Type::Plus, 0) ||
            expectToken(Token::Type::Int, 0) || expectToken(Token::Type::Double, 0)) s_simple_expr();

    int i = 0;
    while(expectToken(Token::Type::Ident, i++))
    {
        if(expectToken(Token::Type::Assignment, i)) assignment();
        else if(expectToken(Token::Type::OpenBracket, i)) s_simple_expr();
        else if(!expectToken(Token::Type::Dot, i++)) s_simple_expr();
        else continue;
        break;
    }

    //throw error
}

void Parser::block_st()
{
    acceptToken(Token::Type::OpenCurly);

    while(!expectToken(Token::Type::CloseCurly, 0)) simple_st();   //TBD

    acceptToken(Token::Type::CloseCurly);
}

void Parser::s_constructor()
{
    acceptToken(Token::Type::Ident);
    acceptToken(Token::Type::OpenBracket);

    if(expectToken(Token::Type::Ident, 0))
    {
        acceptToken(Token::Type::Ident);
        acceptToken(Token::Type::Ident);

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            acceptToken(Token::Type::Ident);
            acceptToken(Token::Type::Ident);
        }
    }

    acceptToken(Token::Type::CloseBracket);
    block_st();
}

void Parser::private_part()
{
    acceptToken(Token::Type::Private);
    acceptToken(Token::Type::Colon);

    while(true)
    {
        if(!expectToken(Token::Type::Ident, 0)) break;

        if(expectToken(Token::Type::OpenBracket, 1)) s_constructor();
        else if(expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) s_function();
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) s_variable();
            else break; 
        }
        else break; 
    }
}

void Parser::public_part()
{
    acceptToken(Token::Type::Public);
    acceptToken(Token::Type::Colon);

    while(true)
    {
        if(!expectToken(Token::Type::Ident, 0)) break;

        if(expectToken(Token::Type::OpenBracket, 1)) s_constructor();
        else if(expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) s_function();
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) s_variable();
            else break; 
        }
        else break; 
    }
}

void Parser::class_content()
{
    while(true)
    {
        if(expectToken(Token::Type::Private, 0)) private_part();
        else if(expectToken(Token::Type::Public, 0)) public_part();
        else break;
    }
}

void Parser::s_class()
{
    acceptToken(Token::Type::Class);
    acceptToken(Token::Type::Ident);
    acceptToken(Token::Type::OpenCurly);
    class_content();
    acceptToken(Token::Type::CloseCurly);
    acceptToken(Token::Type::Semicolon);
}

void Parser::program()//while loop might be faulty (eof)
{
    while(true)
    {
        if(expectToken(Token::Type::Class, 0)) s_class();
        else if(expectToken(Token::Type::Ident, 0) && expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) s_function();
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) s_variable();
            else error_manager.handleError(Error(Error::Type::Unexpected_token, tokens.front().getLine(), tokens.front().getPosition())); 
        }
        else break;
    }
}