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

        bool expectToken(Token::Type type, size_t position);
        bool expectToken(std::set<Token::Type> types, size_t position);

        void acceptToken();
        void acceptToken(Token::Type type);
        void acceptToken(std::set<Token::Type> types);
        
        ast::Node c_ident();
        ast::Node s_variable();
        ast::Node s_function();
        ast::Node comp_expr();
        ast::Node comp_expr2();
        ast::Node expr();
        ast::Node assignment();
        ast::Node return_st();
        ast::Node statement();
        ast::Node while_st();
        ast::Node if_st();
        ast::Node function();
        ast::Node bool_ele();
        ast::Node bool_expr2();
        ast::Node bool_expr();
        ast::Node arithm_ele();
        ast::Node simple_expr2();
        ast::Node simple_expr();
        ast::Node s_simple_expr();
        ast::Node simple_st();
        ast::Node block_st();
        ast::Node s_constructor();
        ast::Node private_part();
        ast::Node public_part();
        ast::Node class_content();
        ast::Node s_class();
        ast::Node program();
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
