#include "headers/parser.hpp"
#include <iostream>

ast::Node Parser::parse()
{
    return program();
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

ast::Node Parser::c_ident()
{
    ast::Node ret(ast::Node::Syntax::C_Ident);

    acceptToken(Token::Type::Ident);

    while(expectToken(Token::Type::Dot, 0))
    {
        acceptToken(Token::Type::Dot);
        acceptToken(Token::Type::Ident);
    }

    return ret;
}

ast::Node Parser::s_variable()
{
    ast::Node ret(ast::Node::Syntax::S_Variable);

    acceptToken(Token::Type::Ident);
    acceptToken(Token::Type::Ident);

    while(expectToken(Token::Type::Comma, 0))
    {
        acceptToken(Token::Type::Comma);
        acceptToken(Token::Type::Ident);
    }

    acceptToken(Token::Type::Semicolon);

    return ret;
}

ast::Node Parser::s_function()
{
    ast::Node ret(ast::Node::Syntax::S_Function);

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
    ret.attachNode( block_st() );

    return ret;
}

ast::Node Parser::comp_expr2()
{
    ast::Node ret(ast::Node::Syntax::Comp_Expr);

    ret.attachNode( simple_expr() );

    if(expectToken(rel_operators, 0))
    {
        acceptToken(rel_operators);
        ret.attachNode( simple_expr() );
    }
    return ret;
}

ast::Node Parser::comp_expr()
{
    ast::Node ret(ast::Node::Syntax::Comp_Expr);

    ret.attachNode( simple_expr() );

    acceptToken(rel_operators);

    ret.attachNode( simple_expr() );

    return ret;
}

ast::Node Parser::expr()
{
    ast::Node ret(ast::Node::Syntax::Expr);

    std::cout << "TODO1" << std::endl;
    ret.attachNode( simple_expr() );
    std::cout << "TODO2" << std::endl;

    return ret;
}

ast::Node Parser::assignment()
{
    ast::Node ret(ast::Node::Syntax::Assignment);

    ret.attachNode( c_ident() );

    acceptToken(Token::Type::Assignment);

    ret.attachNode( expr() );

    acceptToken(Token::Type::Semicolon);

    return ret;
}

ast::Node Parser::return_st()
{
    ast::Node ret(ast::Node::Syntax::Return_St);

    acceptToken(Token::Type::Return);

    if(!expectToken(Token::Type::Semicolon, 0)) ret.attachNode( expr() ); //optional

    acceptToken(Token::Type::Semicolon);

    return ret;
}

ast::Node Parser::statement()
{
    ast::Node ret(ast::Node::Syntax::Statement);

    if(expectToken(Token::Type::OpenCurly, 0)) ret.attachNode( block_st() );
    else ret.attachNode( simple_st() );

    return ret;
}

ast::Node Parser::while_st()
{
    ast::Node ret(ast::Node::Syntax::While_St);

    acceptToken(Token::Type::While);
    acceptToken(Token::Type::OpenBracket);
    ret.attachNode( bool_expr() );
    acceptToken(Token::Type::CloseBracket);

    ret.attachNode( statement() );

    return ret;
}

ast::Node Parser::if_st()
{
    ast::Node ret(ast::Node::Syntax::If_St);

    acceptToken(Token::Type::If);
    acceptToken(Token::Type::OpenBracket);
    ret.attachNode( bool_expr() );
    acceptToken(Token::Type::CloseBracket);

    ret.attachNode( statement() );

    if(expectToken(Token::Type::Else, 0))
    {
        acceptToken(Token::Type::Else);
        ret.attachNode( statement() );
    }

    return ret;
}

ast::Node Parser::function()
{
    ast::Node ret(ast::Node::Syntax::Function);

    ret.attachNode( c_ident() );
    acceptToken(Token::Type::OpenBracket);

    if(!expectToken(Token::Type::CloseBracket, 0))
    {
        ret.attachNode( expr() );

        while(expectToken(Token::Type::Comma, 0))
        {
            acceptToken(Token::Type::Comma);
            ret.attachNode( expr() );
        }
    }

    acceptToken(Token::Type::CloseBracket);

    return ret;
}

ast::Node Parser::bool_ele() //TBD
{
    ast::Node ret(ast::Node::Syntax::Bool_Ele);

    if(expectToken(Token::Type::Not,0)) acceptToken(Token::Type::Not);

    if(expectToken(Token::Type::Bool, 0)) acceptToken(Token::Type::Bool);
    else
    {
        ret.attachNode( comp_expr2() );   //TODO

        /*int i = 0;
        while(expectToken(Token::Type::Ident, i++)) //TODO
        {
            if(expectToken(Token::Type::OpenBracket, i)) function();
            else if(!expectToken(Token::Type::Dot, i++)) c_ident();
            else continue;
            break;
        }*/
    }

    return ret;
}

ast::Node Parser::bool_expr2()
{
    ast::Node ret(ast::Node::Syntax::Bool_Expr2);

    ret.attachNode( bool_ele() );

    while(expectToken(Token::Type::And, 0))
    {
        acceptToken(Token::Type::And);
        ret.attachNode( bool_ele() );
    }

    return ret;
}

ast::Node Parser::bool_expr()
{
    ast::Node ret(ast::Node::Syntax::Bool_Expr);

    ret.attachNode( bool_expr2() );

    while(expectToken(Token::Type::Or, 0))
    {
        acceptToken(Token::Type::Or);
        ret.attachNode( bool_expr2() );
    }

    return ret;
}

ast::Node Parser::arithm_ele()
{
    ast::Node ret(ast::Node::Syntax::Arithm_Ele);

    if(expectToken(sign,0)) acceptToken(sign);

    if(expectToken(const_arithm, 0)) acceptToken(const_arithm);
    else
    {
        int i = 0;
        while(expectToken(Token::Type::Ident, i++))//TODO
        {
            if(expectToken(Token::Type::OpenBracket, i)) ret.attachNode( function() );
            else if(!expectToken(Token::Type::Dot, i++)) ret.attachNode( c_ident() );
            else continue;
            break;
        }
    }

    return ret;
}

ast::Node Parser::simple_expr2()
{
    ast::Node ret(ast::Node::Syntax::Simple_Expr2);

    ret.attachNode( arithm_ele() );

    while(expectToken(mul_operators, 0))
    {
        acceptToken(mul_operators);
        ret.attachNode( arithm_ele() );
    }

    return ret;
}

ast::Node Parser::simple_expr()
{
    ast::Node ret(ast::Node::Syntax::Simple_Expr);

    ret.attachNode( simple_expr2() );

    while(expectToken(add_operators, 0))
    {
        acceptToken(add_operators);
        ret.attachNode( simple_expr2() );
    }

    return ret;
}

ast::Node Parser::s_simple_expr()
{
    ast::Node ret = simple_expr();
    acceptToken(Token::Type::Semicolon);
    return ret;
}

ast::Node Parser::simple_st()
{
    ast::Node ret(ast::Node::Syntax::Simple_St);

    if(expectToken(Token::Type::If, 0)) ret.attachNode( if_st() );
    else if(expectToken(Token::Type::While, 0)) ret.attachNode( while_st() );
    else if(expectToken(Token::Type::Return, 0)) ret.attachNode( return_st() );
    else if(expectToken(Token::Type::Ident, 0) && expectToken(Token::Type::Ident, 1)) ret.attachNode( s_variable() );
    else if(expectToken(Token::Type::Minus, 0) || expectToken(Token::Type::Plus, 0) ||
            expectToken(Token::Type::Int, 0) || expectToken(Token::Type::Double, 0)) ret.attachNode( s_simple_expr() );

    int i = 0;
    while(expectToken(Token::Type::Ident, i++))
    {
        if(expectToken(Token::Type::Assignment, i)) ret.attachNode( assignment() );
        else if(expectToken(Token::Type::OpenBracket, i)) ret.attachNode( s_simple_expr() );
        else if(!expectToken(Token::Type::Dot, i++)) ret.attachNode( s_simple_expr() );
        else continue;
        break;
    }

    //error_manager.handleError(Error(Error::Type::Unexpected_token, peekToken().getLine(), peekToken().getPosition()));   //yikes
    //throw error
    return ret;
}

ast::Node Parser::block_st()
{
    ast::Node ret(ast::Node::Syntax::Block_St);

    acceptToken(Token::Type::OpenCurly);

    while(!expectToken(Token::Type::CloseCurly, 0)) ret.attachNode( simple_st() );   //TBD

    acceptToken(Token::Type::CloseCurly);

    return ret;
}

ast::Node Parser::s_constructor()
{
    ast::Node ret(ast::Node::Syntax::S_Constructor);

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
    ret.attachNode( block_st() );

    return ret;
}

ast::Node Parser::private_part()
{
    ast::Node ret(ast::Node::Syntax::Private_Part);

    acceptToken(Token::Type::Private);
    acceptToken(Token::Type::Colon);

    while(true)
    {
        if(!expectToken(Token::Type::Ident, 0)) break;

        if(expectToken(Token::Type::OpenBracket, 1)) ret.attachNode( s_constructor() );
        else if(expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) ret.attachNode( s_function() );
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) ret.attachNode( s_variable() );
            else break; 
        }
        else break; 
    }

    return ret;
}

ast::Node Parser::public_part()
{
    ast::Node ret(ast::Node::Syntax::Public_Part);

    acceptToken(Token::Type::Public);
    acceptToken(Token::Type::Colon);

    while(true)
    {
        if(!expectToken(Token::Type::Ident, 0)) break;

        if(expectToken(Token::Type::OpenBracket, 1)) ret.attachNode( s_constructor() );
        else if(expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) ret.attachNode( s_function() );
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) ret.attachNode( s_variable() );
            else break; 
        }
        else break; 
    }

    return ret;
}

ast::Node Parser::class_content()
{
    ast::Node ret(ast::Node::Syntax::Class_Content);

    while(true)
    {
        if(expectToken(Token::Type::Private, 0)) ret.attachNode( private_part() );
        else if(expectToken(Token::Type::Public, 0)) ret.attachNode( public_part() );
        else break;
    }

    return ret;
}

ast::Node Parser::s_class()
{
    ast::Node ret(ast::Node::Syntax::S_Class);

    acceptToken(Token::Type::Class);
    acceptToken(Token::Type::Ident);
    acceptToken(Token::Type::OpenCurly);
    ret.attachNode( class_content() );
    acceptToken(Token::Type::CloseCurly);
    acceptToken(Token::Type::Semicolon);

    return ret;
}

ast::Node Parser::program()//while loop might be faulty (eof)
{
    ast::Node ret(ast::Node::Syntax::Program);

    while(true)
    {
        if(expectToken(Token::Type::Class, 0)) ret.attachNode( s_class() );
        else if(expectToken(Token::Type::Ident, 0) && expectToken(Token::Type::Ident, 1))
        {
            if(expectToken(Token::Type::OpenBracket, 2)) ret.attachNode( s_function() );
            else if(expectToken(Token::Type::Comma, 2) || expectToken(Token::Type::Semicolon, 2)) ret.attachNode( s_variable() );
            else error_manager.handleError(Error(Error::Type::Unexpected_token, tokens.front().getLine(), tokens.front().getPosition())); 
        }
        else break;
    }

    return ret;
}