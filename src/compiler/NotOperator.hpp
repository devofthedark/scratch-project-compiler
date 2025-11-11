#include <memory>

#include "Expression.hpp"
#include "compiler/ASTNode.hpp"

class NotOperator : public Expression {
private:
    std::unique_ptr<Expression> operand;

public:
    explicit NotOperator(std::unique_ptr<Expression> operand);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    void print(int depth = 0, std::string prefix = "") override;
    std::unique_ptr<Expression> make_expression_compat(
        StatementSubstitution &statements_added) override;
    std::unique_ptr<Expression> conv_name(const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
};