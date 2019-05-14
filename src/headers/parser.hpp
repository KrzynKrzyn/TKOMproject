#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include <deque>
#include <set>

#include "token.hpp"
#include "lexer.hpp"
#include "error_manager.hpp"
#include "ast.hpp"

class Parser
{
    public:
        ast::Node parse();

        Parser(Lexer &l, ErrorManager &em);

    private:
        Lexer &lexer;
        ErrorManager &error_manager;

        std::deque<Token> tokens;
        //size_t parser_position = 0;
        std::set<Token::Type> sign = { Token::Type::Plus, Token::Type::Minus };
        std::set<Token::Type> const_arithm = { Token::Type::Int, Token::Type::Double };
        std::set<Token::Type> add_operators = { Token::Type::Plus, Token::Type::Minus };
        std::set<Token::Type> mul_operators = { Token::Type::Multiplication, Token::Type::Division, Token::Type::Modulus };
        std::set<Token::Type> rel_operators = { Token::Type::Equals, Token::Type::Less, Token::Type::Greater,
                                                Token::Type::NEqual, Token::Type::LEqual, Token::Type::GEqual };

        Token nextToken();
        Token peekToken();

        bool expectToken(Token::Type type, size_t position = 0);
        bool expectToken(std::set<Token::Type> types, size_t position = 0);

        Token acceptToken();
        Token acceptToken(Token::Type type);
        Token acceptToken(std::set<Token::Type> types);
        
        void c_ident(ast::Node& n);
        void s_variable(ast::Node& n);
        void s_function(ast::Node& n);
        void comp_expr(ast::Node& n);
        void started_comp_expr(ast::Node& n);
        void expr(ast::Node& n);
        void assignment(ast::Node& n);
        void return_st(ast::Node& n);
        void statement(ast::Node& n);
        void while_st(ast::Node& n);
        void if_st(ast::Node& n);
        void function(ast::Node& n);
        void bool_ele(ast::Node& n);
        void bool_expr2(ast::Node& n);
        void bool_expr(ast::Node& n);
        void arithm_ele(ast::Node& n);
        void simple_expr2(ast::Node& n);
        void simple_expr(ast::Node& n);
        void s_simple_expr(ast::Node& n);
        void simple_st(ast::Node& n);
        void block_st(ast::Node& n);
        void s_constructor(ast::Node& n);
        void private_part(ast::Node& n);
        void public_part(ast::Node& n);
        void class_content(ast::Node& n);
        void s_class(ast::Node& n);
        void program(ast::Node& n);
/*
        Token peekToken()
        {
            while(parser_position >= tokens.size()) tokens.push_back(lexer.getToken());

            return tokens[parser_position];
        }

        Token getToken()
        {
            Token ret = peekToken();

            ++parser_position;

            return ret;
        }

        bool isToken(Token::Type expected)
        {
            return peekToken().getType() == expected;
        }

        bool acceptToken(Token::Type expected)
        {
            bool ret;
            if(ret = isToken(expected)) getToken();

            return ret;
        }

        void c_ident()
        {
            acceptToken(Token::Type::Ident);

            if(isToken(Token::Type::Comma))
            {
                acceptToken(Token::Type::Comma);
                acceptToken(Token::Type::Ident);
            }
        }

        bool arithm_ele()
        {
            if(isToken(Token::Type::Plus) || isToken(Token::Type::Minus)) getToken();



        }
*/
};

#endif // PARSER_HPP_INCLUDED
