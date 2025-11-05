#include "NotOperator.hpp"

#include <iostream>

NotOperator::NotOperator(std::unique_ptr<Expression> operand) : operand(std::move(operand)) {}

Type NotOperator::typeCheck(TypeCheckerContext &ctx) const {
    if (operand->typeCheck(ctx) != Type::BOOL) {
        return Type::ERROR;
    }
    return Type::BOOL;
}

void NotOperator::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "Bool Not\n";
    operand->print(depth + 1, "operand: ");
}

std::string NotOperator::compile(json &work) const {
    std::string operand_id = operand->compile(work);
    std::string node_id = generate_id();
    work[node_id] = {{"opcode", "operator_not"},
                     {"inputs", {{"OPERAND", {2, operand_id}}}},
                     {"fields", json::object()},
                     {"topLevel", false},
                     {"shadow", false}};
    return node_id;
}