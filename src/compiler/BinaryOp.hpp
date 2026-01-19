#pragma once
#include <memory>
#include <stdexcept>

#include "Expression.hpp"
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

inline std::string op_str(BinaryOperator opr) {
    switch (opr) {
        case BinaryOperator::ADD:
            return "+";
        case BinaryOperator::SUBTRACT:
            return "-";
        case BinaryOperator::MULTIPLY:
            return "*";
        case BinaryOperator::DIVIDE:
            return "/";
        case BinaryOperator::MODULO:
            return "%";
        case BinaryOperator::AND:
            return "&&";
        case BinaryOperator::OR:
            return "||";
        case BinaryOperator::EQUAL:
            return "==";
        case BinaryOperator::NOT_EQUAL:
            return "!=";
        case BinaryOperator::GREATER_THAN:
            return ">";
        case BinaryOperator::LESS_THAN:
            return "<";
        case BinaryOperator::GREATER_THAN_EQUAL:
            return ">=";
        case BinaryOperator::LESS_THAN_EQUAL:
            return "<=";
    }
    throw std::runtime_error("fatal error");
}
class BinaryOp : public Expression {
private:
    std::unique_ptr<Expression> left, right;
    BinaryOperator op;
    [[nodiscard]] std::string opcode() const;
    [[nodiscard]] std::string input_name() const;
    [[nodiscard]] bool is_compound_operator() const;
    [[nodiscard]] bool is_boolean_operator() const;

public:
    BinaryOp(std::unique_ptr<Expression> _left,
             std::unique_ptr<Expression> _right,
             BinaryOperator _op);
    [[nodiscard]] const Expression *getLeft() const;
    [[nodiscard]] const Expression *getRight() const;
    [[nodiscard]] BinaryOperator getOperator() const;
    Type typeCheck(TypeCheckerContext &ctx) override;
    void print(int depth = 0, std::string prefix = "") override;
    std::unique_ptr<Expression> make_expression_compat(
        const std::string &sprite_name, StatementSubstitution &statements_added) override;
    std::unique_ptr<Expression> conv_name(const std::set<std::string> &args = {}) override;
    std::string compile(json &work) const override;
};
BinaryOperator token_to_binary_operator(TokenType type);
