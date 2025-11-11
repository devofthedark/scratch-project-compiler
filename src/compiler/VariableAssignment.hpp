#pragma once
#include <memory>

#include "Expression.hpp"
#include "Statement.hpp"
class VariableAssignment : public Statement {
protected:
    // Design is intentional, stop clang-tidy from complaining
    // NOLINTBEGIN
    std::string name;
    std::unique_ptr<Expression> value;
    // NOLINTEND

public:
    VariableAssignment(std::string name, std::unique_ptr<Expression> value);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    std::unique_ptr<Expression> release_value();
    StatementSubstitution make_statement_compat(const std::set<std::string> &args = {}) override;
    void print(int depth = 0, std::string prefix = "") override;
    std::string compile(json &work) const override;
};