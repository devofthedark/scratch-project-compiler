
#include "Expression.hpp"

class StdcallStatement : public Statement {
private:
    std::vector<std::unique_ptr<Expression>> args;
    std::vector<std::string> passed_args;

public:
    explicit StdcallStatement(std::vector<std::unique_ptr<Expression>> args)
        : args(std::move(args)) {};
    std::string compile(json &work) const override;
    [[nodiscard]] bool is_stdcall() const override {
        return true;
    }
    void add_arg_to_stdcall(std::string new_arg) override {
        passed_args.emplace_back(std::move(new_arg));
    }
};