#include "VariableAssignment.hpp"

#include <format>
#include <iostream>

#include "compiler/ASTNode.hpp"
#include "compiler/Statement.hpp"
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

StatementSubstitution VariableAssignment::make_statement_compat(const std::set<std::string> &args) {
    StatementSubstitution return_value = {.new_statements = {},
                                          .tmp_variables = 0,
                                          .replace_orig = false};
    if (args.contains(name)) {
        throw std::runtime_error(
            std::format("Variable \"{}\" cannot be assigned to as it's a function parameter\n",
                        name));
    }
    replace_if_valid(value, value->conv_name(args));
    replace_if_valid(value, value->make_expression_compat(return_value));
    return return_value;
}

void VariableAssignment::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "VariableAssignment( " << name << " )\n";
    value->print(depth + 1, "Value: ");
}

std::string VariableAssignment::compile(json &work) const {
    std::string expr_id = value->compile(work);
    std::string var_id = generate_id();
    work[var_id] = {{"opcode", "data_setvariableto"},
                    {"inputs", {{"VALUE", num_value(expr_id)}}},
                    {"fields", {{"VARIABLE", json::array({name, name})}}},
                    {"shadow", false},
                    {"topLevel", false}};
    return var_id;
}