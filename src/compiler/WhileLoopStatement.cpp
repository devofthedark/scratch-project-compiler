#include "WhileLoopStatement.hpp"

#include <format>
#include <iostream>

#include "compiler/NotOperator.hpp"
#include "exceptions/LanguageErrors.hpp"

WhileLoopStatement::WhileLoopStatement(std::unique_ptr<Expression> _condition,
                                       std::unique_ptr<BlockStatement> _body)
    : condition(std::move(_condition)), body(std::move(_body)) {}
Type WhileLoopStatement::typeCheck(TypeCheckerContext &ctx) {
    // Check the condition
    Type cond_type = condition->typeCheck(ctx);
    if (cond_type != Type::BOOL) {
        throw TypeError(
            std::format("Condition for while statement is of wrong type, expected bool, got {}.",
                        type_str(cond_type)));
    }
    return body->typeCheck(ctx);
}
void WhileLoopStatement::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "WhileLoopStatement\n";
    condition->print(depth + 1, "Condition: ");
    body->print(depth + 1, "Body: ");
}

StatementSubstitution WhileLoopStatement::make_statement_compat(const std::string &sprite_name,
                                                                const std::set<std::string> &args) {
    StatementSubstitution return_value = {
        .new_statements = {},
        .tmp_variables = body->make_statement_compat(sprite_name, args).tmp_variables,
        .replace_orig = false};
    replace_if_valid(condition, condition->conv_name(args));
    replace_if_valid(condition, condition->make_expression_compat(sprite_name, return_value));
    /* Scratch uses "repeat until" for while loops
       Thus while(a) is equivalant to repeat until(!a)
    */
    replace_if_valid(condition, std::make_unique<NotOperator>(std::move(condition)));
    return return_value;
}

std::string WhileLoopStatement::compile(json &work) const {
    std::string condition_id = condition->compile(work);
    std::string body_id = body->compile(work);
    // Scratch uses "repeat until" for while loops, the preprocessing required is done
    std::string node_id = generate_id();
    work[node_id] =
        json::object({{"opcode", "control_repeat_until"},
                      {"inputs",
                       json::object({json::object({"CONDITION", json::array({2, condition_id})}),
                                     json::object({"SUBSTACK", json::array({2, body_id})})})},
                      {"fields", json::object()},
                      {"topLevel", false}});
    return node_id;
}