#include "Expression.hpp"
#include "Statement.hpp"
#include <memory>
class FunctionExpression : public Expression, public Statement {
    private:
        std::string name;
        std::vector<std::unique_ptr<Expression>> args;
    public:
        FunctionExpression(std::string name, std::vector<std::unique_ptr<Expression>> args);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] const std::vector<std::unique_ptr<Expression>>& getArgs() const;
        std::vector<Type> getArgTypes(TypeCheckerContext &ctx) const;
        void print(int depth = 0, std::string prefix = "") override;
};