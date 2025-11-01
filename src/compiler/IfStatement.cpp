#include "IfStatement.hpp"

#include <iostream>

#include "compiler/ASTNode.hpp"

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
std::string IfStatement::compile(json &work) const {
    std::string condition_id = condition->compile(work);
    std::string true_id = trueBlock->compile(work);
    std::string if_id = generate_id();
    work[if_id] = {{"opcode", "control_if"},
                   {"inputs",
                    {{"CONDITION", json::array({2, condition_id})},
                     {"SUBSTACK", json::array({2, true_id})}}},
                   {"fields", json::object()},
                   {"next", nullptr},
                   {"topLevel", false},
                   {"shadow", false},
                   {"id", if_id}};
    return if_id;
}