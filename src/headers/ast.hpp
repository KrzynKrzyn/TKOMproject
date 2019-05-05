#ifndef AST_HPP_INCLUDED
#define AST_HPP_INCLUDED

#include <vector>
#include "token.hpp"

namespace ast{

class Node
{
    public:
        enum class Syntax
        {
            Program,
            S_Variable,
            S_Class,
            S_Function,
            Type,
            Class_Content,
            Private_Part,
            Public_Part,
            S_Constructor,
            C_Ident,
            Function,
            Statement,
            Simple_St,
            Block_St,
            If_St,
            While_St,
            Expr,
            Comp_Expr,
            Simple_Expr,
            Simple_Expr2,
            Arithm_Ele,
            Bool_Expr,
            Bool_Expr2,
            Bool_Ele,
            Assignment,
            Return_St,
        };

    private:
        class Data
        {
            bool is_token;

            Node &n;
            Token &t;

            public:
                Data(Node &n_): is_token(false), n(n_) {}
                Data(Token &t_): is_token(true), t(t_) {}
        };

        Node::Syntax type;
        std::vector<Data> children;
        
};
}
#endif // AST_HPP_INCLUDED
