#include "compiler/ReturnStatement.hpp"

#include <iostream>

#include "compiler/VariableAssignment.hpp"

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> _return_value)
    : return_value(std::move(_return_value)) {}

Type ReturnStatement::typeCheck(TypeCheckerContext &ctx) {
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
    if (return_value) {
        replace_if_valid(return_value, return_value->conv_name(args));
        replace_if_valid(return_value, return_value->make_expression_compat(ret_value));
        if (return_value->is_stdcall_hook()) {
            return ret_value;
        }
        ret_value.new_statements.push_back(
            std::make_unique<VariableAssignment>("__scratch_compiler_function_return_value",
                                                 std::move(return_value)));
    }
    return ret_value;
}

std::string ReturnStatement::compile(json &work) const {
    std::string stop_id = generate_id();
    work[stop_id] = json::object(
        {{"opcode", "control_stop"},
         {"next", nullptr},
         {"inputs", json::object()},
         {"fields", json::object({{"STOP_OPTION", json::array({"this script", nullptr})}})},
         {"shadow", false},
         {"topLevel", false},
         {"mutation",
          json::object(
              {{"tagname", "mutation"}, {"children", json::array()}, {"hasnext", "false"}})}});
    return stop_id;
}