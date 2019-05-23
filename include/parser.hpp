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
        void printAST();

        Parser(Lexer &l, ErrorManager &em);

    private:
        Lexer &lexer;
        ErrorManager &error_manager;

        std::deque<Token> tokens;
        
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

        void signalError();
        
        void c_ident(ast::Node& n);
        void s_variable(ast::Node& n);
        void s_function(ast::Node& n);
        void comp_expr(ast::Node& n, ast::Node backtracking = ast::Node::empty);
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
        void arithm_ele(ast::Node& n, ast::Node backtracking = ast::Node::empty);
        void simple_expr2(ast::Node& n, ast::Node backtracking = ast::Node::empty);
        void simple_expr(ast::Node& n, ast::Node backtracking = ast::Node::empty);
        void s_simple_expr(ast::Node& n);
        void simple_st(ast::Node& n);
        void block_st(ast::Node& n);
        void s_constructor(ast::Node& n);
        void private_part(ast::Node& n);
        void public_part(ast::Node& n);
        void class_content(ast::Node& n);
        void s_class(ast::Node& n);
        void program(ast::Node& n);

        ast::Node root = ast::Node("Root", 0, 0);
};

#endif // PARSER_HPP_INCLUDED
