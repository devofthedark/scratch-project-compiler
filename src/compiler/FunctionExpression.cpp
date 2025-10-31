#include <iostream>
#include "FunctionExpression.hpp"
#include "compiler/ASTNode.hpp"

FunctionExpression::FunctionExpression(std::string _name, std::vector<std::unique_ptr<Expression>> _args)
    : name(std::move(_name)), args(std::move(_args)) {}
Type FunctionExpression::typeCheck(TypeCheckerContext &ctx) const {
    // Check if the function exists
    const FunctionSignature* funcSig = ctx.lookupFunction(name, getArgTypes(ctx));
    if (!funcSig) {
        return Type::ERROR;
    }

    // Check the argument types
    for (size_t i = 0; i < args.size(); ++i) {
        Type argType = args[i]->typeCheck(ctx);
        if (argType != funcSig->argTypes[i]) {
            return Type::ERROR;
        }
    }

    return funcSig->returnType;
}
const std::string& FunctionExpression::getName() const {
    return name;
}
const std::vector<std::unique_ptr<Expression>>& FunctionExpression::getArgs() const {
    return args;
}
std::vector<Type> FunctionExpression::getArgTypes(TypeCheckerContext &ctx) const {
    std::vector<Type> argTypes;
    for (const auto& arg : args) {
        argTypes.push_back(arg->typeCheck(ctx));
    }
    return argTypes;
}
void FunctionExpression::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "FunctionExpression( " << name << " )\n";
    for (size_t i = 0; i < args.size(); ++i) {
        args[i]->print(depth + 1, "[" + std::to_string(i) + "] ");
    }
}