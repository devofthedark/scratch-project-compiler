#include "compiler/ReturnStatement.hpp"

#include <iostream>

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