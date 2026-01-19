#include "VariableExpression.hpp"

#include <format>
#include <iostream>

#include "FunctionArgument.hpp"
#include "exceptions/LanguageErrors.hpp"
VariableExpression::VariableExpression(std::string _name) : name(std::move(_name)) {}
Type VariableExpression::typeCheck(TypeCheckerContext &ctx) {
    auto tmp = ctx.lookupVariable(name);
    if (tmp == Type::ERROR) {
        throw TypeError(std::format("Variable \"{}\" does not exist.", name));
    }
    return tmp;
}
const std::string &VariableExpression::getName() const {
    return name;
}
void VariableExpression::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "VariableExpression( " << name << " )\n";
}

std::unique_ptr<Expression> VariableExpression::conv_name(const std::set<std::string> &args) {
    if (args.contains(name)) {
        return std::make_unique<FunctionArgument>(name);
    }
    return nullptr;
}

std::string VariableExpression::compile(json &work) const {
    (void) work;
    return "[12,\"" + name + "\", \"" + name + "\"]";
}