#ifndef AST_HPP_INCLUDED
#define AST_HPP_INCLUDED

#include <memory>
#include <vector>
#include "token.hpp"

namespace ast{

class Node
{
    public:
        enum class Syntax
        {
            Program = 0,
            S_Variable = 1,
            S_Class = 2,
            S_Function = 3,
            Type = 4,
            Class_Content = 5,
            Private_Part = 6,
            Public_Part = 7,
            S_Constructor = 8,
            C_Ident = 9,
            Function = 10,
            Statement = 11,
            Simple_St = 12,
            Block_St = 13,
            If_St = 14,
            While_St = 15,
            Expr = 16,
            Comp_Expr = 17,
            Simple_Expr = 18,
            Simple_Expr2 = 19,
            Arithm_Ele = 20,
            Bool_Expr = 21,
            Bool_Expr2 = 22,
            Bool_Ele = 23,
            Assignment = 24,
            Return_St = 25,

            Null = 26,
        };

    //private:
    /*
        class Data
        {
            bool is_token;

            std::unique_ptr<Node> n;
            Token t;

            public:
                bool isToken() const { return is_token; }

                //Data(Node &n_): is_token(false), n(n_) {}
                Data(Token t_): is_token(true), t(t_) {}
        };*/

        Node::Syntax type;
        std::vector<Node> children;

    public:

        void attachNode(Node &&n)
        {
            children.push_back(std::move(n));
        }

        Node(Syntax s = Syntax::Null): type(s) {}
};
}
#endif // AST_HPP_INCLUDED
