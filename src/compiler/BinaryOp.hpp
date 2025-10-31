#pragma once
#include <memory>
#include "Expression.hpp"
#include "compiler/ASTNode.hpp"
#include "compiler/Lexer.hpp"
enum class BinaryOperator : uint8_t {
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
        [[nodiscard]] std::string opcode() const;
        [[nodiscard]] std::string input_name() const;
        [[nodiscard]] bool is_compound_operator() const;
        [[nodiscard]] bool is_boolean_operator() const;
    public:
        BinaryOp(std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right, BinaryOperator _op);
        [[nodiscard]] const Expression* getLeft() const;
        [[nodiscard]] const Expression* getRight() const;
        [[nodiscard]] BinaryOperator getOperator() const;
        Type typeCheck(TypeCheckerContext &ctx) const override;
        void print(int depth = 0, std::string prefix = "") override;
        std::string compile(json &work) const override;
};
BinaryOperator token_to_binary_operator(TokenType type);
