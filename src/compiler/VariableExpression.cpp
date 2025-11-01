#include "VariableExpression.hpp"

#include <iostream>
VariableExpression::VariableExpression(std::string _name) : name(std::move(_name)) {}
Type VariableExpression::typeCheck(TypeCheckerContext &ctx) const {
    return ctx.lookupVariable(name);
}
const std::string &VariableExpression::getName() const {
    return name;
}
void VariableExpression::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "VariableExpression( " << name << " )\n";
}
std::string VariableExpression::compile(json &work) const {
    (void) work;
    return "[12,\"" + name + "\", \"" + name + "\"]";
}