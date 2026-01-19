#include "FunctionExpression.hpp"

#include <cstddef>
#include <format>
#include <iostream>

#include "compiler/ASTNode.hpp"
#include "compiler/StdcallExpression.hpp"
#include "compiler/StdcallStatement.hpp"
#include "compiler/VariableAssignment.hpp"
#include "compiler/VariableExpression.hpp"
#include "exceptions/LanguageErrors.hpp"

FunctionExpression::FunctionExpression(std::string _name,
                                       std::vector<std::unique_ptr<Expression>> _args)
    : name(std::move(_name)), args(std::move(_args)) {}
Type FunctionExpression::typeCheck(TypeCheckerContext &ctx) {
    if (name == "__scratch_stdcall_hook_void") {
        is_void_stdcall_hook = true;
        return Type::VOID;
    }
    if (name == "__scratch_stdcall_hook_return") {
        is_return_stdcall_hook = true;
        return Type::DOUBLE;
    }
    // Check if the function exists
    std::vector<Type> arg_types;
    try {
        arg_types = getArgTypes(ctx);
    } catch (TypeError &e) {
        std::cerr << e.what() << '\n';
        throw TypeError(std::format("In attempted function call to \"{}\"", name));
    }
    f_sig_str = func_sig_str(name, arg_types);
    const FunctionSignature *function_sig = ctx.lookupFunction(name, arg_types);
    if (function_sig == nullptr) {
        throw TypeError(
            std::format("Attempted call to {} failed: function does not exist.", f_sig_str));
    }

    is_stdcall_call = function_sig->is_stdcall;
    if (is_stdcall_call) {
        implementation = function_sig->implementation;
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
    const std::string &sprite_name, const std::set<std::string> &names) {
    StatementSubstitution return_value = {.new_statements = {},
                                          .tmp_variables = 0,
                                          .replace_orig = false};
    for (auto &arg : args) {
        auto tmp = arg->conv_name(names);
        if (tmp) {
            arg = std::move(tmp);
        }
        tmp = arg->make_expression_compat(sprite_name, return_value);
        if (tmp) {
            arg = std::move(tmp);
        }
    }
    if (is_void_stdcall_hook) {
        return_value.new_statements.push_back(std::make_unique<StdcallHook>(std::move(args)));
        return_value.replace_orig = true;
    }
    return return_value;
}
std::unique_ptr<Expression> FunctionExpression::make_expression_compat(
    const std::string &sprite_name, StatementSubstitution &statements_added) {
    for (auto &arg : args) {
        replace_if_valid(arg, arg->make_expression_compat(sprite_name, statements_added));
    }
    if (is_return_stdcall_hook) {
        return std::make_unique<StdcallExpression>(std::move(args));
    }
    if (is_stdcall_call) {
        return nullptr;
    }
    std::string tmp_var_name = std::format("__scratch_compiler_tmp_var_{}_{}",
                                           statements_added.tmp_variables++,
                                           sprite_name);
    statements_added.new_statements.push_back(
        std::make_unique<FunctionExpression>(std::move(name), std::move(args)));
    statements_added.new_statements.push_back(std::make_unique<VariableAssignment>(
        tmp_var_name,
        std::make_unique<VariableExpression>(
            std::format("__scratch_compiler_function_return_value_{}", sprite_name))));
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
    // stdcall handling
    if (is_stdcall_call) {
        try {
            const auto *const IMPL = implementation->getFirstStatement();
            const auto *const EXPR = (*IMPL)->expr_if_return_statement();
            if (EXPR != nullptr) {
                (*EXPR)->clear_stdcall_args();
                for (const auto &arg : args) {
                    (*EXPR)->add_arg_to_stdcall(arg->compile(work));
                }
                return (*EXPR)->compile(work);
            }
            for (const auto &arg : args) {
                (*IMPL)->add_arg_to_stdcall(arg->compile(work));
            }
            return (*IMPL)->compile(work);
        } catch (TypeError &e) {
            std::cerr << e.what() << '\n';
            throw TypeError(std::format("In call to {}", f_sig_str));
        }
    }
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
    work[call_procedure] = json::object({{"opcode", "procedures_call"},
                                         {"inputs", inputs},
                                         {"fields", json::object()},
                                         {"shadow", false},
                                         {"topLevel", false},
                                         {"mutation",
                                          json::object({{"tagName", "mutation"},
                                                        {"children", json::array()},
                                                        {"proccode", proccode},
                                                        {"argumentids", argids.dump()},
                                                        {"warp", "false"}})}});
    return call_procedure;
}
