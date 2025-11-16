#pragma once
#include "Expression.hpp"
class LiteralNumberNode : public Expression {
private:
    double value;
    [[nodiscard]] std::string clean_to_string() const;

public:
    explicit LiteralNumberNode(double value);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    [[nodiscard]] double getValue() const;
    void print(int depth = 0, std::string prefix = "") override;
    std::string compile(json &work) const override;
};
class LiteralStringNode : public Expression {
private:
    std::string value;

public:
    explicit LiteralStringNode(const std::string &value);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    [[nodiscard]] const std::string &getValue() const;
    void print(int depth = 0, std::string prefix = "") override;
};
class LiteralBoolNode : public Expression {
private:
    bool value;

public:
    explicit LiteralBoolNode(bool value);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    [[nodiscard]] bool getValue() const;
    void print(int depth = 0, std::string prefix = "") override;
};