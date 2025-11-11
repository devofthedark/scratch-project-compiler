#include "BlockStatement.hpp"
#include "Expression.hpp"
#include "compiler/Statement.hpp"

class IfStatement : public Statement {
private:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> trueBlock;
    std::unique_ptr<BlockStatement> falseBlock;

public:
    IfStatement(std::unique_ptr<Expression> condition,
                std::unique_ptr<BlockStatement> trueBlock,
                std::unique_ptr<BlockStatement> falseBlock);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    void print(int depth = 0, std::string prefix = "") override;
    StatementSubstitution make_statement_compat(const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
};