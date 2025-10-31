#include <iostream>
#include "FunctionExpression.hpp"
#include "compiler/ASTNode.hpp"

FunctionExpression::FunctionExpression(std::string _name, std::vector<std::unique_ptr<Expression>> _args)
    : name(std::move(_name)), args(std::move(_args)) {}
Type FunctionExpression::typeCheck(TypeCheckerContext &ctx) const {
    // Check if the function exists
    const FunctionSignature* function_sig = ctx.lookupFunction(name, getArgTypes(ctx));
    if (function_sig == nullptr) {
        return Type::ERROR;
    }

    // Check the argument types
    for (size_t i = 0; i < args.size(); ++i) {
        Type arg_type = args[i]->typeCheck(ctx);
        if (arg_type != function_sig->argTypes[i]) {
            return Type::ERROR;
        }
    }

    return function_sig->returnType;
}
const std::string& FunctionExpression::getName() const {
    return name;
}
const std::vector<std::unique_ptr<Expression>>& FunctionExpression::getArgs() const {
    return args;
}
std::vector<Type> FunctionExpression::getArgTypes(TypeCheckerContext &ctx) const {
    std::vector<Type> arg_types;
    arg_types.reserve(args.size());
    for (const auto& arg : args) {
        arg_types.push_back(arg->typeCheck(ctx));
    }
    return arg_types;
}
void FunctionExpression::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "FunctionExpression( " << name << " )\n";
    for (size_t i = 0; i < args.size(); ++i) {
        args[i]->print(depth + 1, "[" + std::to_string(i) + "] ");
    }
}