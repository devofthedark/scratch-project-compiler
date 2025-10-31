#include "Expression.hpp"
class VariableExpression : public Expression {
    private:
        std::string name;

    public:
        VariableExpression(std::string name);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        const std::string& getName() const;
        void print(int depth = 0, std::string prefix = "") override;
        std::string compile(json &work) const override;
};