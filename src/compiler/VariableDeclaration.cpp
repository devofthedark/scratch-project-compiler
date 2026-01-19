#include "VariableDeclaration.hpp"

#include <format>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

#include "exceptions/LanguageErrors.hpp"

VariableDeclaration::VariableDeclaration(std::string _name,
                                         std::unique_ptr<Expression> _value,
                                         Type _type)
    : VariableAssignment(std::move(_name), std::move(_value)), type(_type) {}
VariableDeclaration::VariableDeclaration(VariableAssignment &assignment, Type _type)
    : VariableAssignment(std::move(assignment)), type(_type) {}
Type VariableDeclaration::typeCheck(TypeCheckerContext &ctx) {
    // Check if the variable name is already used
    if (ctx.lookupVariable(name) != Type::ERROR) {
        throw TypeError(std::format("Cannot redeclare already existing variable \"{}\".", name));
    }
    if (name.starts_with("id_")) {
        throw TypeError("Variables beginning with \"id_\" are reserved.");
    }
    if (name.starts_with("__")) {
        throw TypeError("Variables beginning with \"__\" are reserved.");
    }
    // Check the initializer
    Type init_type = Type::ERROR;
    try {
        init_type = value->typeCheck(ctx);
    } catch (TypeError &e) {
        std::cerr << e.what() << '\n';
        throw TypeError(std::format("In declaration of variable \"{}\"", name));
    }
    assert(init_type != Type::ERROR);
    // Check if the initializer type matches the variable type
    if (init_type != type) {
        throw TypeError(std::format("Cannot assign value of type {} to variable \"{}\" of type {}.",
                                    type_str(init_type),
                                    name,
                                    type_str(type)));
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
