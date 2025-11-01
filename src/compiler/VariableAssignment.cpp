#include "VariableAssignment.hpp"

#include <iostream>
VariableAssignment::VariableAssignment(std::string _name, std::unique_ptr<Expression> _value)
    : name(std::move(_name)), value(std::move(_value)) {}
Type VariableAssignment::typeCheck(TypeCheckerContext &ctx) const {
    // Check if the variable exists and is the correct type
    Type var_type = ctx.lookupVariable(name);
    if (var_type == Type::ERROR) {
        return Type::ERROR;
    }
    if (var_type != value->typeCheck(ctx)) {
        return Type::ERROR;
    }
    return Type::VOID;
}

std::unique_ptr<Expression> VariableAssignment::release_value() {
    return std::move(value);
}
void VariableAssignment::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "VariableAssignment( " << name << " )\n";
    value->print(depth + 1, "Value: ");
}
std::string VariableAssignment::compile(json &work) const {
    std::string expr_id = value->compile(work);
    std::string var_id = generate_id();
    work[var_id] = {{"opcode", "data_setvariableto"},
                    {"inputs",
                     {{"VALUE",
                       {3,
                        expr_id,
                        {10, // NOLINT TODO: find constants??
                         "0"}}}}},
                    {"fields", {{"VARIABLE", {name, name}}}},
                    {"shadow", false},
                    {"topLevel", false}};
    return var_id;
}