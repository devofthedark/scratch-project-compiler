#pragma once
#include "BlockStatement.hpp"

struct Parameter {
    std::string name;
    Type type;
};

class FunctionDeclaration : public Statement {
private:
    std::string name;
    std::shared_ptr<BlockStatement> body;
    std::vector<Parameter> parameters;
    Type returnType;
    bool is_stdcall;

public:
    FunctionDeclaration(std::string name,
                        std::shared_ptr<BlockStatement> body,
                        std::vector<Parameter> parameters,
                        Type returnType,
                        bool is_stdcall = false);
    Type typeCheck(TypeCheckerContext &ctx) override;
    [[nodiscard]] const std::string &getName() const;
    void print(int depth = 0, std::string prefix = "") override;
    StatementSubstitution make_statement_compat(const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
};