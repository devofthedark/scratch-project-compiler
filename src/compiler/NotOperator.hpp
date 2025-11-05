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
    std::string compile(json &work) const override;
};