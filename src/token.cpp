#include "headers/token.hpp"

std::map<Token::Type, std::string> Token::type_names =
{
    {Token::Type::Ident, "Identifier"},
    {Token::Type::Int, "Int"},
    {Token::Type::Double, "Double"},
    {Token::Type::Bool, "Bool"},

    {Token::Type::If, "If"},
    {Token::Type::Else, "Else"},
    {Token::Type::While, "While"},
    {Token::Type::Class, "Class"},
    {Token::Type::Public, "Public"},
    {Token::Type::Private, "Private"},
    {Token::Type::Return, "Return"},

    {Token::Type::Equals, "Equals"},
    {Token::Type::NEqual, "Not Equal"},
    {Token::Type::GEqual, "Greater or equal"},
    {Token::Type::LEqual, "Less or equal"},
    {Token::Type::Greater, "Greater"},
    {Token::Type::Less, "Less"},
    {Token::Type::And, "And"},
    {Token::Type::Or, "Or"},
    {Token::Type::Not, "Not"},

    {Token::Type::Assignment, "Assignment"},
    {Token::Type::Multiplication, "Multiplication"},
    {Token::Type::Division, "Division"},
    {Token::Type::Plus, "Plus"},
    {Token::Type::Minus, "Minus"},
    {Token::Type::Modulus, "Modulus"},

    {Token::Type::OpenBracket, "Open bracket"},
    {Token::Type::CloseBracket, "Close bracket"},
    {Token::Type::OpenCurly, "Open curly"},
    {Token::Type::CloseCurly, "Close curly"},

    {Token::Type::Dot, "Dot"},
    {Token::Type::Comma, "Comma"},
    {Token::Type::Semicolon, "Semicolon"},
    {Token::Type::Colon, "Colon"},

    {Token::Type::Eof, "Eof"},
    {Token::Type::Broken, "Broken"},
};

std::string Token::toString()
{
    auto to_string = type_names.find(this->getType());
    if(to_string == type_names.end()) return "Unknown token";

    if(this->getType() == Token::Type::Ident) return to_string->second + " (" + this->getName() + ")";
    if(this->getType() == Token::Type::Int) return to_string->second + " (" + std::to_string(this->getInt()) + ")";
    if(this->getType() == Token::Type::Double) return to_string->second + " (" + std::to_string(this->getDouble()) + ")";
    if(this->getType() == Token::Type::Bool) return to_string->second + " (" + std::to_string(this->getBool()) + ")";

    return to_string->second;
}
