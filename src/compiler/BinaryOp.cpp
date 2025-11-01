#include "BinaryOp.hpp"

#include <iostream>

#include "compiler/ASTNode.hpp"

BinaryOp::BinaryOp(std::unique_ptr<Expression> _left,
                   std::unique_ptr<Expression> _right,
                   BinaryOperator _op)
    : left(std::move(_left)), right(std::move(_right)), op(_op) {}
const Expression *BinaryOp::getLeft() const {
    return left.get();
}
const Expression *BinaryOp::getRight() const {
    return right.get();
}
BinaryOperator BinaryOp::getOperator() const {
    return op;
}
Type BinaryOp::typeCheck(TypeCheckerContext &ctx) const {
    Type left_type = left->typeCheck(ctx);
    Type right_type = right->typeCheck(ctx);

    if (left_type == Type::ERROR || right_type == Type::ERROR) {
        return Type::ERROR;
    }

    switch (op) {
        case BinaryOperator::ADD:
        case BinaryOperator::SUBTRACT:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO:
            return (left_type == Type::DOUBLE && right_type == Type::DOUBLE) ? Type::DOUBLE
                                                                             : Type::ERROR;
        case BinaryOperator::AND:
        case BinaryOperator::OR:
            return (left_type == Type::BOOL && right_type == Type::BOOL) ? Type::BOOL : Type::ERROR;
        case BinaryOperator::EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::GREATER_THAN_EQUAL:
        case BinaryOperator::LESS_THAN_EQUAL:
            return (left_type == Type::DOUBLE && right_type == Type::DOUBLE) ? Type::BOOL
                                                                             : Type::ERROR;
    }
    return Type::ERROR; // Fallback
}
void BinaryOp::print(int depth, std::string prefix) {
    std::string op_str;
    switch (op) {
        case BinaryOperator::ADD:
            op_str = "+";
            break;
        case BinaryOperator::SUBTRACT:
            op_str = "-";
            break;
        case BinaryOperator::MULTIPLY:
            op_str = "*";
            break;
        case BinaryOperator::DIVIDE:
            op_str = "/";
            break;
        case BinaryOperator::MODULO:
            op_str = "%";
            break;
        case BinaryOperator::AND:
            op_str = "&&";
            break;
        case BinaryOperator::OR:
            op_str = "||";
            break;
        case BinaryOperator::EQUAL:
            op_str = "==";
            break;
        case BinaryOperator::NOT_EQUAL:
            op_str = "!=";
            break;
        case BinaryOperator::GREATER_THAN:
            op_str = ">";
            break;
        case BinaryOperator::LESS_THAN:
            op_str = "<";
            break;
        case BinaryOperator::GREATER_THAN_EQUAL:
            op_str = ">=";
            break;
        case BinaryOperator::LESS_THAN_EQUAL:
            op_str = "<=";
            break;
    }
    std::cout << depth_prefix(depth, prefix) << "BinaryOp( " << op_str << " )\n";
    left->print(depth + 1, "left: ");
    right->print(depth + 1, "right: ");
}
BinaryOperator token_to_binary_operator(TokenType type) {
    switch (type) {
        case TokenType::PLUS:
            return BinaryOperator::ADD;
        case TokenType::MINUS:
            return BinaryOperator::SUBTRACT;
        case TokenType::MULTIPLY:
            return BinaryOperator::MULTIPLY;
        case TokenType::DIVIDE:
            return BinaryOperator::DIVIDE;
        case TokenType::MOD:
            return BinaryOperator::MODULO;
        case TokenType::AND:
            return BinaryOperator::AND;
        case TokenType::OR:
            return BinaryOperator::OR;
        case TokenType::EQUALS:
            return BinaryOperator::EQUAL;
        case TokenType::NOT_EQUALS:
            return BinaryOperator::NOT_EQUAL;
        case TokenType::GREATER_THAN:
            return BinaryOperator::GREATER_THAN;
        case TokenType::LESS_THAN:
            return BinaryOperator::LESS_THAN;
        case TokenType::GREATER_EQUAL:
            return BinaryOperator::GREATER_THAN_EQUAL;
        case TokenType::LESS_EQUAL:
            return BinaryOperator::LESS_THAN_EQUAL;
        default:
            throw std::runtime_error("Invalid token type for binary operator conversion");
    }
}
std::string BinaryOp::opcode() const {
    switch (op) {
        case BinaryOperator::ADD:
            return "operator_add";
        case BinaryOperator::SUBTRACT:
            return "operator_subtract";
        case BinaryOperator::MULTIPLY:
            return "operator_multiply";
        case BinaryOperator::DIVIDE:
            return "operator_divide";
        case BinaryOperator::MODULO:
            return "operator_mod";
        case BinaryOperator::AND:
            return "operator_and";
        case BinaryOperator::OR:
            return "operator_or";
        case BinaryOperator::EQUAL:
            return "operator_equals";
        // not equals is part of "compound operators"
        case BinaryOperator::GREATER_THAN:
            return "operator_gt";
        case BinaryOperator::LESS_THAN:
            return "operator_lt";
        default:
            break;
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
    return op == BinaryOperator::NOT_EQUAL || op == BinaryOperator::GREATER_THAN_EQUAL
        || op == BinaryOperator::LESS_THAN_EQUAL;
}
bool BinaryOp::is_boolean_operator() const {
    return op == BinaryOperator::AND || op == BinaryOperator::OR || op == BinaryOperator::EQUAL
        || op == BinaryOperator::NOT_EQUAL || op == BinaryOperator::GREATER_THAN
        || op == BinaryOperator::LESS_THAN || op == BinaryOperator::GREATER_THAN_EQUAL
        || op == BinaryOperator::LESS_THAN_EQUAL;
}
namespace {
json num_value(std::string scratch_id, bool is_boolean) {
    if (scratch_id[0] == '[' && scratch_id[1] == '4') {
        return json::array({1, json::parse(scratch_id)});
    }
    if (scratch_id[0] == '[' && scratch_id[1] == '1') {
        return json::array({3, json::parse(scratch_id), json::array({4, "0"})});
    }
    if (is_boolean) {
        return json::array({2, scratch_id});
    }
    return json::array({3, scratch_id, json::array({4, "0"})});
}
} // namespace
std::string BinaryOp::compile(json &work) const {
    std::string left_id = left->compile(work);
    std::string right_id = right->compile(work);
    std::string node_id = generate_id();
    bool is_boolean = is_boolean_operator();
    if (!is_compound_operator()) {
        work[node_id] = {{"opcode", opcode()},
                         {"inputs",
                          {{input_name() + "1", num_value(left_id, is_boolean)},
                           {input_name() + "2", num_value(right_id, is_boolean)}}},
                         {"fields", json::object()},
                         {"topLevel", false},
                         {"shadow", false}};
    } else {
        // Compound operator handling
        std::string base_node_id = generate_id();
        std::string base_opcode;
        if (op == BinaryOperator::NOT_EQUAL) {
            base_opcode = "operator_equals";
        } else if (op == BinaryOperator::GREATER_THAN_EQUAL) {
            base_opcode = "operator_lt";
        } else {
            base_opcode = "operator_gt";
        }
        work[base_node_id] = {{"opcode", base_opcode},
                              {"inputs",
                               {{input_name() + "1", num_value(left_id, is_boolean)},
                                {input_name() + "2", num_value(right_id, is_boolean)}}},
                              {"fields", json::object()},
                              {"topLevel", false},
                              {"shadow", false}};
        // Now create a NOT operator node
        work[node_id] = {{"opcode", "operator_not"},
                         {"inputs", {{"OPERAND", {2, base_node_id}}}},
                         {"fields", json::object()},
                         {"topLevel", false},
                         {"shadow", false}};
    }
    return node_id;
}