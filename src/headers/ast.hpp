#ifndef AST_HPP_INCLUDED
#define AST_HPP_INCLUDED

#include <vector>
#include "token.hpp"

namespace ast{

class Node
{
    public:


    private:
        std::vector<Node> children;
        Token token;
};
}
#endif // AST_HPP_INCLUDED
