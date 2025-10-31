#pragma once
#include "Expression.hpp"
#include <string>
class LiteralNumberNode : public Expression {
    private:
        double value;
        std::string clean_to_string() const;
    public:
        LiteralNumberNode(double value);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        double getValue() const;
        void print(int depth = 0, std::string prefix = "") override;
        std::string compile(json &work) const override;

};
class LiteralStringNode : public Expression {
    private:
        std::string value;
    public:
        LiteralStringNode(const std::string& value);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        const std::string& getValue() const;
        void print(int depth = 0, std::string prefix = "") override;

};
class LiteralBoolNode : public Expression {
    private:
        bool value;
    public:
        LiteralBoolNode(bool value);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        bool getValue() const;
        void print(int depth = 0, std::string prefix = "") override;

};