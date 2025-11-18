#include "VariableDeclaration.hpp"

#include <iostream>
#include <magic_enum/magic_enum.hpp>

VariableDeclaration::VariableDeclaration(std::string _name,
                                         std::unique_ptr<Expression> _value,
                                         Type _type)
    : VariableAssignment(std::move(_name), std::move(_value)), type(_type) {}
VariableDeclaration::VariableDeclaration(VariableAssignment &assignment, Type _type)
    : VariableAssignment(std::move(assignment)), type(_type) {}
Type VariableDeclaration::typeCheck(TypeCheckerContext &ctx) const {
    // Check if the variable name is already used
    if (ctx.lookupVariable(name) != Type::ERROR) {
        return Type::ERROR;
    }
    // Check the initializer
    Type init_type = value->typeCheck(ctx);
    if (init_type == Type::ERROR) {
        return Type::ERROR;
    }
    // Check if the initializer type matches the variable type
    if (init_type != type) {
        return Type::ERROR;
    }
    // Add the variable to the context
    ctx.addVariable(name, type);
    return Type::VOID;
}

void VariableDeclaration::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "VariableDeclaration( " << name << " )\n";
    if (value) {
        value->print(depth + 1, "Initializer: ");
    }
    std::cout << depth_prefix(depth + 1, "Type: ") << magic_enum::enum_name(type) << "\n";
}
