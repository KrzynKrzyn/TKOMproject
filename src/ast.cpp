#include "ast.hpp"

using ast::Node;

Node::Node(std::string name_, int row_, int pos_, std::string value_): 
    production(name_, value_, row_, pos_) {}

const Node Node::empty = Node("",0,0);

bool Node::isEmpty()
{
    return production.name == "" && children.size() == 0;
}

void Node::adjustLevels()
{
    for(Node& n : children) 
    {
        n.level = level + 1;
        n.adjustLevels();
    }
}

Node& Node::attachNode(std::string name_, int row_, int pos_, std::string value_)
{
    children.push_back(Node(name_, row_, pos_, value_)); //std::move?

    children.rbegin()->level = this->level + 1;

    return children.back();
}

Node& Node::attachNode(Node&& n)
{
    children.push_back(std::move(n));

    children.rbegin()->level = this->level + 1;
    children.back().adjustLevels();

    return children.back();
}

void Node::printTree()
{
    for(int i=0; i<level; ++i) std::cout << '\t';
    std::cout << production.name;
    if(!production.value.empty()) std::cout << " (" << production.value << ")";
    std::cout << std::endl;

    for(Node n : children) n.printTree();
}