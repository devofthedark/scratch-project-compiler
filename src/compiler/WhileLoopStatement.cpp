#include "WhileLoopStatement.hpp"

#include <iostream>
WhileLoopStatement::WhileLoopStatement(std::unique_ptr<Expression> _condition,
                                       std::unique_ptr<BlockStatement> _body)
    : condition(std::move(_condition)), body(std::move(_body)) {}
Type WhileLoopStatement::typeCheck(TypeCheckerContext &ctx) const {
    // Check the condition
    Type cond_type = condition->typeCheck(ctx);
    if (cond_type != Type::BOOL) {
        return Type::ERROR;
    }
    return body->typeCheck(ctx);
}
void WhileLoopStatement::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "WhileLoopStatement\n";
    condition->print(depth + 1, "Condition: ");
    body->print(depth + 1, "Body: ");
}
std::string WhileLoopStatement::compile(json &work) const {
    std::string condition_id = condition->compile(work);
    std::string body_id = body->compile(work);
    // Scratch uses "repeat until" for while loops
    std::string base_condition_id = generate_id();
    work[base_condition_id] = {{"opcode", "operator_not"},
                               {"inputs", {{"OPERAND", {2, condition_id}}}},
                               {"fields", json::object()},
                               {"topLevel", false},
                               {"shadow", false}};
    std::string node_id = generate_id();
    work[node_id] = {{"opcode", "control_repeat_until"},
                     {"inputs",
                      {{"CONDITION", {2, base_condition_id}}, {"SUBSTACK", {2, body_id}}}},
                     {"fields", json::object()},
                     {"topLevel", false}};
    return node_id;
}