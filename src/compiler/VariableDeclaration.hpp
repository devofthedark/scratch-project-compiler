#pragma once
#include "VariableAssignment.hpp"

class VariableDeclaration : public VariableAssignment {
private:
    Type type;

public:
    VariableDeclaration(std::string _name, std::unique_ptr<Expression> _value, Type _type);
    VariableDeclaration(VariableAssignment &assignment, Type type);
    Type typeCheck(TypeCheckerContext &ctx) override;
    void print(int depth = 0, std::string prefix = "") override;
};