#pragma once
#include "Expression.hpp"

class VariableExpression : public Expression {
private:
    std::string name;

public:
    explicit VariableExpression(std::string name);
    Type typeCheck(TypeCheckerContext &ctx) override;
    [[nodiscard]] const std::string &getName() const;
    void print(int depth = 0, std::string prefix = "") override;
    std::unique_ptr<Expression> conv_name(const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
};