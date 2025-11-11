#pragma once
#include "VariableExpression.hpp"

class FunctionArgument : public VariableExpression {
public:
    explicit FunctionArgument(std::string name);
    std::string compile(json &work) const override;
};