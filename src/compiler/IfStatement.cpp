#include "IfStatement.hpp"

#include <iostream>

IfStatement::IfStatement(std::unique_ptr<Expression> _condition,
                         std::unique_ptr<BlockStatement> _trueBlock,
                         std::unique_ptr<BlockStatement> _falseBlock)
    : condition(std::move(_condition)), trueBlock(std::move(_trueBlock)),
      falseBlock(std::move(_falseBlock)) {}
Type IfStatement::typeCheck(TypeCheckerContext &ctx) const {
    if (condition->typeCheck(ctx) != Type::BOOL) {
        return Type::ERROR;
    }
    if (trueBlock->typeCheck(ctx) == Type::ERROR) {
        return Type::ERROR;
    }
    if (falseBlock && falseBlock->typeCheck(ctx) == Type::ERROR) {
        return Type::ERROR;
    }
    return Type::VOID;
}
void IfStatement::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "IfStatement\n";
    condition->print(depth + 1, "Condition: ");
    trueBlock->print(depth + 1, "True Block: ");
    if (falseBlock) {
        falseBlock->print(depth + 1, "False Block: ");
    } else {
        std::cout << depth_prefix(depth + 1, "False Block: ") << "None\n";
    }
}

StatementSubstitution IfStatement::make_statement_compat(const std::set<std::string> &args) {
    StatementSubstitution return_value = {.new_statements = {},
                                          .tmp_variables =
                                              trueBlock->make_statement_compat(args).tmp_variables,
                                          .replace_orig = false};
    if (falseBlock) {
        return_value.tmp_variables = std::max(falseBlock->make_statement_compat(args).tmp_variables,
                                              return_value.tmp_variables);
    }
    replace_if_valid(condition, condition->conv_name(args));
    replace_if_valid(condition, condition->conv_name(args));
    return return_value;
}

std::string IfStatement::compile(json &work) const {
    std::string condition_id = condition->compile(work);
    std::string true_id = trueBlock->compile(work);
    std::string if_id = generate_id();
    work[if_id] = json::object({{"opcode", "control_if"},
                                {"inputs",
                                 json::object({{"CONDITION", json::array({2, condition_id})},
                                               {"SUBSTACK", json::array({2, true_id})}})},
                                {"fields", json::object()},
                                {"next", nullptr},
                                {"topLevel", false},
                                {"shadow", false}});
    return if_id;
}