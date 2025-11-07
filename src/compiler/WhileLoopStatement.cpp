#include "WhileLoopStatement.hpp"

#include <iostream>

#include "compiler/NotOperator.hpp"
#include "compiler/Statement.hpp"

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

StatementSubstitution WhileLoopStatement::make_statement_compat() {
    StatementSubstitution return_value = {.new_statements = {},
                                          .tmp_variables = 0,
                                          .replace_orig = false};
    auto tmp = condition->make_expression_compat(return_value);
    if (tmp) {
        condition = std::move(tmp);
    }
    /* Scratch uses "repeat until" for while loops
       Thus while(a) is equivalant to repeat until(!a)
    */
    tmp = std::make_unique<NotOperator>(std::move(condition));
    condition = std::move(tmp);
    return return_value;
}

std::string WhileLoopStatement::compile(json &work) const {
    std::string condition_id = condition->compile(work);
    std::string body_id = body->compile(work);
    // Scratch uses "repeat until" for while loops, the preprocessing required is done
    std::string node_id = generate_id();
    work[node_id] = {{"opcode", "control_repeat_until"},
                     {"inputs", {{"CONDITION", {2, condition_id}}, {"SUBSTACK", {2, body_id}}}},
                     {"fields", json::object()},
                     {"topLevel", false}};
    return node_id;
}