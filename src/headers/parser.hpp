#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include <deque>
#include <set>

#include "token.hpp"
#include "lexer.hpp"
#include "error_manager.hpp"

class Parser
{
    public:
        void parse();

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
        
        void c_ident();
        void s_variable();
        void s_function();
        void comp_expr();
        void expr();
        void assignment();
        void return_st();
        void statement();
        void while_st();
        void if_st();
        void function();
        void bool_ele();
        void bool_expr2();
        void bool_expr();
        void arithm_ele();
        void simple_expr2();
        void simple_expr();
        void s_simple_expr();
        void simple_st();
        void block_st();
        void s_constructor();
        void private_part();
        void public_part();
        void class_content();
        void s_class();
        void program();
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
