#include "BinaryOp.hpp"

#include <format>
#include <iostream>
#include <memory>

#include "compiler/ASTNode.hpp"
#include "compiler/NotOperator.hpp"
#include "compiler/Statement.hpp"
#include "exceptions/LanguageErrors.hpp"

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
Type BinaryOp::typeCheck(TypeCheckerContext &ctx) {
    Type left_type = left->typeCheck(ctx);
    Type right_type = right->typeCheck(ctx);
    Type expected_type = Type::BOOL;

    assert(left_type != Type::ERROR && right_type != Type::ERROR);

    switch (op) {
        case BinaryOperator::ADD:
        case BinaryOperator::SUBTRACT:
        case BinaryOperator::MULTIPLY:
        case BinaryOperator::DIVIDE:
        case BinaryOperator::MODULO:
            expected_type = Type::DOUBLE;
            if (left_type != Type::DOUBLE || right_type != Type::DOUBLE) {
                throw TypeError(std::format("Invalid types for operator {0}: expected num {0} num, "
                                            "got {1} {0} {2} instead.",
                                            op_str(op),
                                            type_str(left_type),
                                            type_str(right_type)));
            }
            break;
        case BinaryOperator::EQUAL:
        case BinaryOperator::NOT_EQUAL:
        case BinaryOperator::GREATER_THAN:
        case BinaryOperator::LESS_THAN:
        case BinaryOperator::GREATER_THAN_EQUAL:
        case BinaryOperator::LESS_THAN_EQUAL:
            if (left_type != Type::DOUBLE || right_type != Type::DOUBLE) {
                throw TypeError(std::format("Invalid types for operator {0}: expected num {0} num, "
                                            "got {1} {0} {2} instead.",
                                            op_str(op),
                                            type_str(left_type),
                                            type_str(right_type)));
            }
            break;
        case BinaryOperator::AND:
        case BinaryOperator::OR:
            if (left_type != Type::BOOL || right_type != Type::BOOL) {
                throw TypeError(
                    std::format("Invalid types for operator {0}: expected bool {0} bool, "
                                "got {1} {0} {2} instead.",
                                op_str(op),
                                type_str(left_type),
                                type_str(right_type)));
            }
            break;
    }
    return expected_type;
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
        case TokenType::EQUALS_TEST:
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

std::unique_ptr<Expression> BinaryOp::make_expression_compat(
    const std::string &sprite_name, StatementSubstitution &statements_added) {
    replace_if_valid(left, left->make_expression_compat(sprite_name, statements_added));
    replace_if_valid(right, right->make_expression_compat(sprite_name, statements_added));
    std::cout << "make_expression_compat invoked\n";
    if (op == BinaryOperator::NOT_EQUAL) {
        return std::make_unique<NotOperator>(
            std::make_unique<BinaryOp>(std::move(left), std::move(right), BinaryOperator::EQUAL));
    }
    if (op == BinaryOperator::LESS_THAN_EQUAL) {
        return std::make_unique<NotOperator>(std::make_unique<BinaryOp>(
            std::move(left), std::move(right), BinaryOperator::GREATER_THAN));
    }
    if (op == BinaryOperator::GREATER_THAN_EQUAL) {
        return std::make_unique<NotOperator>(std::make_unique<BinaryOp>(std::move(left),
                                                                        std::move(right),
                                                                        BinaryOperator::LESS_THAN));
    }
    return nullptr;
}

std::unique_ptr<Expression> BinaryOp::conv_name(const std::set<std::string> &args) {
    replace_if_valid(left, left->conv_name(args));
    replace_if_valid(right, right->conv_name(args));
    return nullptr;
}

std::string BinaryOp::compile(json &work) const {
    std::string left_id = left->compile(work);
    std::string right_id = right->compile(work);
    std::string node_id = generate_id();
    bool is_boolean = is_boolean_operator();
    if (!is_compound_operator()) {
        work[node_id] =
            json::object({{"opcode", opcode()},
                          {"inputs",
                           json::object({{input_name() + "1", num_value(left_id, is_boolean)},
                                         {input_name() + "2", num_value(right_id, is_boolean)}})},
                          {"fields", json::object()},
                          {"topLevel", false},
                          {"shadow", false}});
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
        work[base_node_id] =
            json::object({{"opcode", base_opcode},
                          {"inputs",
                           json::object({{input_name() + "1", num_value(left_id, is_boolean)},
                                         {input_name() + "2", num_value(right_id, is_boolean)}})},
                          {"fields", json::object()},
                          {"topLevel", false},
                          {"shadow", false}});
        // Now create a NOT operator node
        work[node_id] = json::object({{"opcode", "operator_not"},
                                      {"inputs", json::object({{"OPERAND", {2, base_node_id}}})},
                                      {"fields", json::object()},
                                      {"topLevel", false},
                                      {"shadow", false}});
    }
    return node_id;
}
