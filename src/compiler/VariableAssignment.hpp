#pragma once
#include "Statement.hpp"
#include "Expression.hpp"
#include <memory>
class VariableAssignment : public Statement {
    protected:
        std::string name;
        std::unique_ptr<Expression> value;

    public:
        VariableAssignment(std::string name, std::unique_ptr<Expression> value);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        std::unique_ptr<Expression> release_value();
        void print(int depth = 0, std::string prefix = "") override;
        std::string compile(json &work) const override;
};