#pragma once
#include <memory>
#include <vector>
#include "Statement.hpp"
#include "compiler/ASTNode.hpp"

class BlockStatement : public Statement {
    private:
        std::vector<std::unique_ptr<Statement>> statements;
    public:
        BlockStatement(std::vector<std::unique_ptr<Statement>> &statements);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        void add(std::unique_ptr<Statement> stmt);
        void print(int depth = 0, std::string prefix = "") override;
        std::string compile(json &work) const override;

};