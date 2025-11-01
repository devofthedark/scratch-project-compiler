#include <memory>

#include "compiler/Expression.hpp"
#include "compiler/Statement.hpp"

class ReturnStatement : public Statement {
private:
    std::unique_ptr<Expression> return_value;

public:
    explicit ReturnStatement(std::unique_ptr<Expression> return_value);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    void print(int depth = 0, std::string prefix = "") override;
};