#pragma once
#include "BlockStatement.hpp"
#include "Expression.hpp"

class FunctionExpression : public Expression, public Statement {
private:
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;
    std::shared_ptr<BlockStatement> implementation = nullptr;
    std::string f_sig_str;
    bool is_void_stdcall_hook = false;
    bool is_return_stdcall_hook = false;
    bool is_stdcall_call = false;

public:
    FunctionExpression(std::string name, std::vector<std::unique_ptr<Expression>> args);
    FunctionExpression(const FunctionExpression &other);
    FunctionExpression &operator=(const FunctionExpression &other);
    ~FunctionExpression() override = default;
    FunctionExpression(FunctionExpression &&other) noexcept = default;
    FunctionExpression &operator=(FunctionExpression &&other) noexcept = default;
    Type typeCheck(TypeCheckerContext &ctx) override;
    [[nodiscard]] const std::string &getName() const;
    [[nodiscard]] const std::vector<std::unique_ptr<Expression>> &getArgs() const;
    std::vector<Type> getArgTypes(TypeCheckerContext &ctx) const;
    void print(int depth = 0, std::string prefix = "") override;
    StatementSubstitution make_statement_compat(const std::string &sprite_name,
                                                const std::set<std::string> &names = {}) override;
    std::unique_ptr<Expression> make_expression_compat(
        const std::string &sprite_name, StatementSubstitution &statements_added) override;
    std::unique_ptr<Expression> conv_name(const std::set<std::string> &names = {}) override;
    std::string compile(json &work) const override;
};
