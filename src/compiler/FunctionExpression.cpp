#include "FunctionExpression.hpp"

#include <format>
#include <iostream>
#include <memory>

#include "compiler/ASTNode.hpp"
#include "compiler/Expression.hpp"
#include "compiler/Statement.hpp"
#include "compiler/VariableAssignment.hpp"
#include "compiler/VariableExpression.hpp"

FunctionExpression::FunctionExpression(std::string _name,
                                       std::vector<std::unique_ptr<Expression>> _args)
    : name(std::move(_name)), args(std::move(_args)) {}
Type FunctionExpression::typeCheck(TypeCheckerContext &ctx) const {
    // Check if the function exists
    const FunctionSignature *function_sig = ctx.lookupFunction(name, getArgTypes(ctx));
    if (function_sig == nullptr) {
        return Type::ERROR;
    }

    // Check the argument types
    for (size_t i = 0; i < args.size(); ++i) {
        Type arg_type = args[i]->typeCheck(ctx);
        if (arg_type != function_sig->argTypes[i]) {
            return Type::ERROR;
        }
    }

    return function_sig->returnType;
}

FunctionExpression::FunctionExpression(const FunctionExpression &other) : name(other.name) {
    args.reserve(other.args.size());
    for (const auto &arg : other.args) {
        args.push_back(std::make_unique<Expression>(*arg));
    }
}

FunctionExpression &FunctionExpression::operator=(const FunctionExpression &other) {
    if (this != &other) {
        name = other.name;
        args.clear();
        args.reserve(other.args.size());
        for (const auto &arg : other.args) {
            args.push_back(std::make_unique<Expression>(*arg));
        }
    }
    return *this;
}

const std::string &FunctionExpression::getName() const {
    return name;
}
const std::vector<std::unique_ptr<Expression>> &FunctionExpression::getArgs() const {
    return args;
}
std::vector<Type> FunctionExpression::getArgTypes(TypeCheckerContext &ctx) const {
    std::vector<Type> arg_types;
    arg_types.reserve(args.size());
    for (const auto &arg : args) {
        arg_types.push_back(arg->typeCheck(ctx));
    }
    return arg_types;
}
void FunctionExpression::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, std::move(prefix)) << "FunctionExpression( " << name << " )\n";
    for (size_t i = 0; i < args.size(); ++i) {
        args[i]->print(depth + 1, "[" + std::to_string(i) + "] ");
    }
}

StatementSubstitution FunctionExpression::make_statement_compat(
    const std::set<std::string> &names) {
    StatementSubstitution return_value = {.new_statements = {},
                                          .tmp_variables = 0,
                                          .replace_orig = false};
    for (auto &arg : args) {
        auto tmp = arg->conv_name(names);
        if (tmp) {
            arg = std::move(tmp);
        }
        tmp = arg->make_expression_compat(return_value);
        if (tmp) {
            arg = std::move(tmp);
        }
    }
    return return_value;
}
std::unique_ptr<Expression> FunctionExpression::make_expression_compat(
    StatementSubstitution &statements_added) {
    for (auto &arg : args) {
        replace_if_valid(arg, arg->make_expression_compat(statements_added));
    }
    std::string tmp_var_name =
        std::format("__scratch_compiler_tmp_var_{}", statements_added.tmp_variables++);
    statements_added.new_statements.push_back(
        std::make_unique<FunctionExpression>(std::move(name), std::move(args)));
    statements_added.new_statements.push_back(std::make_unique<VariableAssignment>(
        tmp_var_name,
        std::make_unique<VariableExpression>("__scratch_compiler_function_return_value")));
    return std::make_unique<VariableExpression>(tmp_var_name);
}

std::unique_ptr<Expression> FunctionExpression::conv_name(const std::set<std::string> &names) {
    for (auto &arg : args) {
        auto tmp = arg->conv_name(names);
        if (tmp) {
            arg = std::move(tmp);
        }
    }
    return nullptr;
}

std::string FunctionExpression::compile(json &work) const {
    // Just the procedure_call block, return value has been handled
    std::string call_procedure = generate_id();
    json argids = json::array();
    json inputs = json::object();
    std::string proccode = name;
    for (size_t i = 0; i < args.size(); ++i) {
        argids.push_back(std::format("arg_{}", i));
        proccode += " %s";
        inputs[argids.back()] = num_value(args[i]->compile(work));
    }
    work[call_procedure] = {{"opcode", "procedures_call"},
                            {"inputs", inputs},
                            {"fields", json::object()},
                            {"shadow", false},
                            {"topLevel", false},
                            {"mutation",
                             {{"tagName", "mutation"},
                              {"children", json::array()},
                              {"proccode", proccode},
                              {"argumentids", argids.dump()},
                              {"warp", "false"}}}};
    return call_procedure;
}