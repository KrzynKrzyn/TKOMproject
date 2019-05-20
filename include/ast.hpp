#ifndef AST_HPP_INCLUDED
#define AST_HPP_INCLUDED

#include <iostream>
#include <vector>
#include "token.hpp"

namespace ast{

class Node
{
    public:
        struct Production
        {
            std::string name;
            std::string value;
            
            int row, pos;

            Production(std::string name_, std::string value_, int row_, int pos_):
                name(name_), value(value_), row(row_), pos(pos_) {}
        };

        int level = 0;
        Production production;
        std::vector<Node> children;

        Node(std::string name_, int row_, int pos_, std::string value_ = "");

        static const Node empty;

        bool isEmpty();

        void adjustLevels();
        Node& attachNode(std::string name_, int row_, int pos_, std::string value_ = "");
        Node& attachNode(Node&& n);

        void printTree();
};

}
#endif // AST_HPP_INCLUDED
