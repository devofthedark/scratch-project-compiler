#pragma once
#include "compiler/Expression.hpp"

class ReturnStatement : public Statement {
private:
    std::unique_ptr<Expression> return_value;

public:
    explicit ReturnStatement(std::unique_ptr<Expression> return_value);
    Type typeCheck(TypeCheckerContext &ctx) override;
    void print(int depth = 0, std::string prefix = "") override;
    StatementSubstitution make_statement_compat(const std::string &sprite_name,
                                                const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
    [[nodiscard]] const std::unique_ptr<Expression> *expr_if_return_statement() const override {
        return &return_value;
    }
};