#pragma once
#include "BlockStatement.hpp"
#include "Expression.hpp"

class WhileLoopStatement : public Statement {
private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> body;

public:
    WhileLoopStatement(std::unique_ptr<Expression> condition, std::unique_ptr<BlockStatement> body);
    Type typeCheck(TypeCheckerContext &ctx) override;
    void print(int depth = 0, std::string prefix = "") override;
    StatementSubstitution make_statement_compat(const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
};
