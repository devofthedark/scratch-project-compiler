#include "Expression.hpp"

class StdcallExpression : public Expression {
private:
    std::vector<std::unique_ptr<Expression>> args;
    std::shared_ptr<BlockStatement> implementation;

public:
    explicit StdcallExpression(std::vector<std::unique_ptr<Expression>> args,
                               std::shared_ptr<BlockStatement> implementation)
        : args(std::move(args)), implementation(std::move(implementation)) {};
    std::string compile(json &work) const override {
        (void) work;
        return "";
    };
    [[nodiscard]] bool is_stdcall() const override {
        return true;
    }
};