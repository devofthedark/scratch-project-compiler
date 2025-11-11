#include "compiler/ReturnStatement.hpp"

#include <iostream>

#include "compiler/ASTNode.hpp"
#include "compiler/Statement.hpp"

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> _return_value)
    : return_value(std::move(_return_value)) {}

Type ReturnStatement::typeCheck(TypeCheckerContext &ctx) const {
    Type ret_type = return_value->typeCheck(ctx);
    Type expected_type = ctx.getExpectedReturnType();
    if (ret_type == Type::ERROR || expected_type == Type::ERROR) {
        return Type::ERROR;
    }
    return ret_type;
}
void ReturnStatement::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "ReturnStatement:\n";
    if (return_value) {
        return_value->print(depth + 1, "Return Value: ");
    } else {
        std::cout << depth_prefix(depth + 1, "Return Value: ") << "nullptr\n";
    }
}

StatementSubstitution ReturnStatement::make_statement_compat(const std::set<std::string> &args) {
    StatementSubstitution ret_value = {.new_statements = {},
                                       .tmp_variables = 0,
                                       .replace_orig = false};
    replace_if_valid(return_value, return_value->conv_name(args));
    replace_if_valid(return_value, return_value->make_expression_compat(ret_value));
    return ret_value;
}

std::string ReturnStatement::compile(json &work) const {
    std::string value_id = return_value->compile(work);
    std::string stop_id = generate_id();
    std::string return_id = generate_id();
    work[stop_id] = {
        {"opcode", "control_stop"},
        {"next", nullptr},
        {"inputs", json::object()},
        {"fields", {{"STOP_OPTION", json::array({"this script", nullptr})}}},
        {"shadow", false},
        {"topLevel", false},
        {"mutation", {{"tagname", "mutation"}, {"children", json::array()}, {"hasnext", "false"}}}};
    work[return_id] = {
        {"opcode", "data_insertatlist"},
        {"inputs",
         {{"ITEM", num_value(value_id)}, // NOLINTNEXTLINE
          {"INDEX", json::array({1, json::array({7, "1"})})}}},
        {"fields",
         {{"LIST",
           json::array({"__scratch_compiler_return_stack", "__scratch_compiler_return_stack"})}}},
        {"shadow", false},
        {"topLevel", false},
        {"next", stop_id}};
    return return_id;
}