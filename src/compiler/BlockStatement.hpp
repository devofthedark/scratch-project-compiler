#pragma once
#include <list>
#include <memory>
#include <vector>

#include "Statement.hpp"
#include "compiler/ASTNode.hpp"

class BlockStatement : public Statement {
private:
    std::list<std::unique_ptr<Statement>> statements;

public:
    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> &statements);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    void add(std::unique_ptr<Statement> stmt);
    void print(int depth = 0, std::string prefix = "") override;
    StatementSubstitution make_statement_compat() override;
    std::string compile(json &work) const override;
};