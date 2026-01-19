#include "NotOperator.hpp"

#include <format>
#include <iostream>

#include "compiler/ASTNode.hpp"
#include "exceptions/LanguageErrors.hpp"

NotOperator::NotOperator(std::unique_ptr<Expression> operand) : operand(std::move(operand)) {}

Type NotOperator::typeCheck(TypeCheckerContext &ctx) {
    auto opr_type = operand->typeCheck(ctx);
    if (opr_type != Type::BOOL) {
        throw TypeError(
            std::format("Cannot apply operator ! on type {}: bool required.", type_str(opr_type)));
    }
    return Type::BOOL;
}

void NotOperator::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "Bool Not\n";
    operand->print(depth + 1, "operand: ");
}

std::unique_ptr<Expression> NotOperator::make_expression_compat(
    const std::string &sprite_name,
    StatementSubstitution &statements_added) {
    replace_if_valid(operand, operand->make_expression_compat(sprite_name, statements_added));
    return nullptr;
}

std::unique_ptr<Expression> NotOperator::conv_name(const std::set<std::string> &args) {
    replace_if_valid(operand, operand->conv_name(args));
    return nullptr;
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
