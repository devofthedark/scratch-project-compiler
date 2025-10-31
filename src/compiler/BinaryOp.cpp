#include <iostream>
#include "BinaryOp.hpp"
#include "compiler/ASTNode.hpp"


BinaryOp::BinaryOp(std::unique_ptr<Expression> _left, std::unique_ptr<Expression> _right, BinaryOperator _op)
    : left(std::move(_left)), right(std::move(_right)), op(_op) {}
const Expression* BinaryOp::getLeft() const { return left.get(); }
const Expression* BinaryOp::getRight() const { return right.get(); }
BinaryOperator BinaryOp::getOperator() const { return op; }
Type BinaryOp::typeCheck(TypeCheckerContext &ctx) const {
    Type leftType = left->typeCheck(ctx);
    Type rightType = right->typeCheck(ctx);

    if (leftType == Type::ERROR || rightType == Type::ERROR) {
        return Type::ERROR;
    }

    switch (op) {
        case BinaryOperator::ADD:
        case BinaryOperator::SUBTRACT:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO:
            return (leftType == Type::DOUBLE && rightType == Type::DOUBLE) ? Type::DOUBLE : Type::ERROR;
        case BinaryOperator::AND:
        case BinaryOperator::OR:
            return (leftType == Type::BOOL && rightType == Type::BOOL) ? Type::BOOL : Type::ERROR;
        case BinaryOperator::EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::GREATER_THAN_EQUAL:
        case BinaryOperator::LESS_THAN_EQUAL:
            return (leftType == Type::DOUBLE && rightType == Type::DOUBLE) ? Type::BOOL : Type::ERROR;
    }
    return Type::ERROR; // Fallback
}
void BinaryOp::print(int depth, std::string prefix) {
    std::string opStr;
    switch (op) {
        case BinaryOperator::ADD: opStr = "+"; break;
        case BinaryOperator::SUBTRACT: opStr = "-"; break;
        case BinaryOperator::MULTIPLY: opStr = "*"; break;
        case BinaryOperator::DIVIDE: opStr = "/"; break;
        case BinaryOperator::MODULO: opStr = "%"; break;
        case BinaryOperator::AND: opStr = "&&"; break;
        case BinaryOperator::OR: opStr = "||"; break;
        case BinaryOperator::EQUAL: opStr = "=="; break;
        case BinaryOperator::NOT_EQUAL: opStr = "!="; break;
        case BinaryOperator::GREATER_THAN: opStr = ">"; break;
        case BinaryOperator::LESS_THAN: opStr = "<"; break;
        case BinaryOperator::GREATER_THAN_EQUAL: opStr = ">="; break;
        case BinaryOperator::LESS_THAN_EQUAL: opStr = "<="; break;
    }
    std::cout << depth_prefix(depth, prefix) << "BinaryOp( " << opStr << " )\n";
    left->print(depth + 1, "left: ");
    right->print(depth + 1, "right: ");
}
BinaryOperator token_to_binary_operator(TokenType type) {
    switch (type) {
        case TokenType::PLUS: return BinaryOperator::ADD;
        case TokenType::MINUS: return BinaryOperator::SUBTRACT;
        case TokenType::MULTIPLY: return BinaryOperator::MULTIPLY;
        case TokenType::DIVIDE: return BinaryOperator::DIVIDE;
        case TokenType::MOD: return BinaryOperator::MODULO;
        case TokenType::AND: return BinaryOperator::AND;
        case TokenType::OR: return BinaryOperator::OR;
        case TokenType::EQUALS: return BinaryOperator::EQUAL;
        case TokenType::NOT_EQUALS: return BinaryOperator::NOT_EQUAL;
        case TokenType::GREATER_THAN: return BinaryOperator::GREATER_THAN;
        case TokenType::LESS_THAN: return BinaryOperator::LESS_THAN;
        case TokenType::GREATER_EQUAL: return BinaryOperator::GREATER_THAN_EQUAL;
        case TokenType::LESS_EQUAL: return BinaryOperator::LESS_THAN_EQUAL;
        default:
            throw std::runtime_error("Invalid token type for binary operator conversion");
    }
}
std::string BinaryOp::opcode() const {
    switch (op) {
        case BinaryOperator::ADD: return "operator_add";
        case BinaryOperator::SUBTRACT: return "operator_subtract";
        case BinaryOperator::MULTIPLY: return "operator_multiply";
        case BinaryOperator::DIVIDE: return "operator_divide";
        case BinaryOperator::MODULO: return "operator_mod";
        case BinaryOperator::AND: return "operator_and";
        case BinaryOperator::OR: return "operator_or";  
        case BinaryOperator::EQUAL: return "operator_equals";
        // not equals is part of "compound operators"
        case BinaryOperator::GREATER_THAN: return "operator_gt";
        case BinaryOperator::LESS_THAN: return "operator_lt";
        default: break;
    }
    throw std::runtime_error("Invalid binary operator");
}
std::string BinaryOp::input_name() const {
    switch (op) {
        case BinaryOperator::ADD:
        case BinaryOperator::SUBTRACT:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO:
            return "NUM";
        case BinaryOperator::AND:
        case BinaryOperator::OR:
        case BinaryOperator::EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::GREATER_THAN_EQUAL:
        case BinaryOperator::LESS_THAN_EQUAL:
            return "OPERAND";
    }
    throw std::runtime_error("Invalid binary operator");
}
bool BinaryOp::is_compound_operator() const {
    return op == BinaryOperator::NOT_EQUAL ||
           op == BinaryOperator::GREATER_THAN_EQUAL ||
           op == BinaryOperator::LESS_THAN_EQUAL;
}
bool BinaryOp::is_boolean_operator() const {
    return op == BinaryOperator::AND ||
           op == BinaryOperator::OR ||
           op == BinaryOperator::EQUAL ||
           op == BinaryOperator::NOT_EQUAL ||
           op == BinaryOperator::GREATER_THAN ||
           op == BinaryOperator::LESS_THAN ||
           op == BinaryOperator::GREATER_THAN_EQUAL ||
           op == BinaryOperator::LESS_THAN_EQUAL;
}
json num_value(std::string id, bool is_boolean) {
    if (id[0] == '[' && id[1] == '4') {
        return json::array({1, json::parse(id)});
    }
    if (id[0] == '[' && id[1] == '1') {
        return json::array({3, json::parse(id), json::array({4, "0"})});
    }
    if (is_boolean) {
        return json::array({2, id});
    }
    return json::array({3, id, json::array({4, "0"})});
}
std::string BinaryOp::compile(json &work) const {
    std::string left_id = left->compile(work);
    std::string right_id = right->compile(work);
    std::string node_id = generate_id();
    bool is_boolean = is_boolean_operator();
    if (!is_compound_operator()) {
        work[node_id] = {
            {"opcode", opcode()},
            {"inputs", {
                {input_name() + "1", num_value(left_id, is_boolean)},
                {input_name() + "2", num_value(right_id, is_boolean)}
            }},
            {"fields", json::object()},
            {"topLevel", false},
            {"shadow", false}
        };
    } else {
        // Compound operator handling
        std::string base_node_id = generate_id();
        work[base_node_id] = {
            {"opcode", (op == BinaryOperator::NOT_EQUAL) ? "operator_equals" :
                        (op == BinaryOperator::GREATER_THAN_EQUAL) ? "operator_lt" : "operator_gt"},
            {"inputs", {
                {input_name() + "1", num_value(left_id, is_boolean)},
                {input_name() + "2", num_value(right_id, is_boolean)}
            }},
            {"fields", json::object()},
            {"topLevel", false},
            {"shadow", false}
        };
        // Now create a NOT operator node
        work[node_id] = {
            {"opcode", "operator_not"},
            {"inputs", {
                {"OPERAND", {2, base_node_id}}
            }},
            {"fields", json::object()},
            {"topLevel", false},
            {"shadow", false}
        };
    }
    return node_id;
}