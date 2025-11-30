#pragma once
#include "Expression.hpp"

class StdcallHook : public Statement, public Expression {
private:
    std::vector<std::unique_ptr<Expression>> args;
    std::vector<std::string> passed_args;

public:
    explicit StdcallHook(std::vector<std::unique_ptr<Expression>> args) : args(std::move(args)) {};
    std::string compile(json &work) const override;
    [[nodiscard]] bool is_stdcall_hook() const override {
        return true;
    }
    void add_arg_to_stdcall(const std::string &new_arg) override {
        passed_args.push_back(new_arg);
    }
    void clear_stdcall_args() override {
        passed_args.clear();
    }
};