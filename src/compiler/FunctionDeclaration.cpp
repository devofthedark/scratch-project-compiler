#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include "FunctionDeclaration.hpp"
#include "compiler/ASTNode.hpp"

FunctionDeclaration::FunctionDeclaration(std::string _name, std::unique_ptr<BlockStatement> _body, std::vector<Parameter> _parameters, Type _returnType)
    : name(std::move(_name)), body(std::move(_body)), parameters(std::move(_parameters)), returnType(_returnType) {}
Type FunctionDeclaration::typeCheck(TypeCheckerContext &ctx) const {
    // Check the return type
    if (returnType == Type::ERROR) {
        return Type::ERROR;
    }
    // Check if the function name is already used
    // Check if name starts with "__"
    if (name.starts_with("__")) {
        return Type::ERROR;
    }

    // Check the parameters
    std::vector<Type> param_types(parameters.size());
    for (const auto& param : parameters) {
        if (param.type == Type::ERROR) {
            return Type::ERROR;
        }
        // Check if the parameter name is already used
        if (ctx.lookupVariable(param.name) != Type::ERROR) {
            return Type::ERROR;
        }
        // Add the parameter to the context
        ctx.addVariable(param.name, param.type);
    }
    if (ctx.lookupFunction(name, param_types) != nullptr) {
        return Type::ERROR;
    }
    // Set return type
    ctx.setExpectedReturnType(returnType);
    
    // Check the function body
    if (body->typeCheck(ctx) == Type::ERROR) {
        return Type::ERROR;
    }
    ctx.setExpectedReturnType(Type::VOID);
    // Remove the parameters from the context
    for (const auto& param : parameters) {
        ctx.removeVariable(param.name);
    }
    // Add the function to the context
    ctx.addFunction(name, param_types, returnType);
    
    return returnType;
}
const std::string& FunctionDeclaration::getName() const {
    return name;
}
void FunctionDeclaration::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "FunctionDeclaration( " << name << " ) -> " << magic_enum::enum_name(returnType) << "\n";
    std::cout << depth_prefix(depth + 1, "Parameters: ");
    for (const auto& param : parameters) {
        std::cout << param.name << ": " << magic_enum::enum_name(param.type) << ", ";
    }
    std::cout << "\n";
    body->print(depth + 1, "Body: ");
}