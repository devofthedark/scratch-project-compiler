#pragma once
#include <memory>
#include "Expression.hpp"
#include "compiler/ASTNode.hpp"
#include "compiler/Lexer.hpp"
enum class BinaryOperator {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
    AND,
    OR,
    EQUAL,
    NOT_EQUAL,
    GREATER_THAN,
    LESS_THAN,
    GREATER_THAN_EQUAL,
    LESS_THAN_EQUAL
};
class BinaryOp : public Expression {
    private:
        std::unique_ptr<Expression> left, right;
        BinaryOperator op;
        std::string opcode() const;
        std::string input_name() const;
        bool is_compound_operator() const;
        bool is_boolean_operator() const;
    public:
        BinaryOp(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, BinaryOperator op);
        const Expression* getLeft() const;
        const Expression* getRight() const;
        BinaryOperator getOperator() const;
        Type typeCheck(TypeCheckerContext &ctx) const override;
        void print(int depth = 0, std::string prefix = "") override;
        std::string compile(json &work) const override;
};
BinaryOperator token_to_binary_operator(TokenType type);
