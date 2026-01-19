#pragma once
#include <list>

#include "Statement.hpp"

class BlockStatement : public Statement {
protected:
    // Intentional
    // NOLINTNEXTLINE
    std::list<std::unique_ptr<Statement>> statements;

public:
    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> &statements);
    Type typeCheck(TypeCheckerContext &ctx) override;
    void print(int depth = 0, std::string prefix = "") override;
    StatementSubstitution make_statement_compat(const std::string &sprite_name,
                                                const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
    [[nodiscard]] size_t size() const {
        return statements.size();
    }
    [[nodiscard]] const std::unique_ptr<Statement> *getFirstStatement() {
        return &statements.front();
    }
};