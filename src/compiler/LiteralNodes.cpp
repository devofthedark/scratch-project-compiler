#include <iostream>
#include <fmt/format.h>
#include "LiteralNodes.hpp"
LiteralNumberNode::LiteralNumberNode(double _value) : value(_value) {}
Type LiteralNumberNode::typeCheck(TypeCheckerContext &ctx) const {
    (void)ctx;
    return Type::DOUBLE;
}
double LiteralNumberNode::getValue() const {
    return value;
}
void LiteralNumberNode::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "LiteralNumberNode( " << value << " )\n";
}
std::string LiteralNumberNode::clean_to_string() const {
    std::string str = fmt::format("{:.15f}", value);
    // Remove trailing zeros and decimal point if not needed
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    if (str.back() == '.') {
        str.pop_back();
    }
    return str;
}
std::string LiteralNumberNode::compile(json &work) const {
    (void)work;
    return "[4,\"" + clean_to_string() + "\"]"; 
}
LiteralStringNode::LiteralStringNode(const std::string& _value) : value(_value) {}
Type LiteralStringNode::typeCheck(TypeCheckerContext &ctx) const {
    (void)ctx;
    return Type::STRING;
}
const std::string& LiteralStringNode::getValue() const {
    return value;
}
void LiteralStringNode::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "LiteralStringNode( \"" << value << "\" )\n";
}
LiteralBoolNode::LiteralBoolNode(bool _value) : value(_value) {}
Type LiteralBoolNode::typeCheck(TypeCheckerContext &ctx) const {
    (void)ctx;
    return Type::BOOL;
}
bool LiteralBoolNode::getValue() const {
    return value;
}
void LiteralBoolNode::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "LiteralBoolNode( " << (value ? "true" : "false") << " )\n";
}