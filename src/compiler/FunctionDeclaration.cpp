#include "FunctionDeclaration.hpp"

#include <cstddef>
#include <format>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

#include "compiler/ASTNode.hpp"
#include "compiler/FunctionArgument.hpp"
#include "compiler/FunctionBody.hpp"
#include "compiler/Statement.hpp"

FunctionDeclaration::FunctionDeclaration(std::string _name,
                                         std::unique_ptr<BlockStatement> _body,
                                         std::vector<Parameter> _parameters,
                                         Type _returnType)
    : name(std::move(_name)), body(std::move(_body)), parameters(std::move(_parameters)),
      returnType(_returnType) {}
Type FunctionDeclaration::typeCheck(TypeCheckerContext &ctx) const {
    // Check the return type
    if (returnType == Type::ERROR) {
        return Type::ERROR;
    }
    // Check if the function name is already used
    // Check if name starts with "__"
    if (name.starts_with("__")) {
        return Type::ERROR;
    }

    // Check the parameters
    std::vector<Type> param_types(parameters.size());
    for (const auto &param : parameters) {
        if (param.type == Type::ERROR) {
            return Type::ERROR;
        }
        // Check if the parameter name is already used
        if (ctx.lookupVariable(param.name) != Type::ERROR) {
            return Type::ERROR;
        }
        // Add the parameter to the context
        ctx.addVariable(param.name, param.type);
    }
    if (ctx.lookupFunction(name, param_types) != nullptr) {
        return Type::ERROR;
    }
    // Set return type
    ctx.setExpectedReturnType(returnType);
    // Add the function to the context
    ctx.addFunction(name, param_types, returnType);

    // Check the function body
    if (body->typeCheck(ctx) == Type::ERROR) {
        return Type::ERROR;
    }
    ctx.setExpectedReturnType(Type::VOID);
    for (const auto &param : parameters) {
        ctx.removeVariable(param.name);
    }

    return returnType;
}
const std::string &FunctionDeclaration::getName() const {
    return name;
}
void FunctionDeclaration::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "FunctionDeclaration( " << name << " ) -> "
              << magic_enum::enum_name(returnType) << "\n";
    std::cout << depth_prefix(depth + 1, "Parameters: ");
    for (const auto &param : parameters) {
        std::cout << param.name << ": " << magic_enum::enum_name(param.type) << ", ";
    }
    std::cout << "\n";
    body->print(depth + 1, "Body: ");
}

StatementSubstitution FunctionDeclaration::make_statement_compat(
    const std::set<std::string> &args) {
    (void) args;
    std::set<std::string> rep;
    for (const auto &param : parameters) {
        rep.insert(param.name);
    }
    auto ret_val = body->make_statement_compat(rep);
    body = std::make_unique<FunctionBody>(std::move(*body));
    return ret_val;
}

std::string FunctionDeclaration::compile(json &work) const {
    std::string body_id = body->compile(work);
    std::string prototype_id = generate_id();
    std::string def_id = generate_id();
    json argids = json::array();
    json arg_names = json::array();
    json inputs = json::object();
    std::string proccode = name;
    for (size_t i = 0; i < parameters.size(); ++i) {
        argids.push_back(std::format("arg_{}", i));
        arg_names.push_back(parameters[i].name);
        proccode += " %s";
        std::string tmp = FunctionArgument(parameters[i].name).compile(work);
        inputs[argids.back()] = json::array({1, tmp});
        work[tmp]["shadow"] = true;
    }
    work[prototype_id] = {{"opcode", "procedures_prototype"},
                          {"next", nullptr},
                          {"inputs", inputs},
                          {"fields", json::object()},
                          {"shadow", true},
                          {"topLevel", false},
                          {"mutation",
                           {{"tagName", "mutation"},
                            {"children", json::array()},
                            {"proccode", proccode},
                            {"argumentids", argids.dump()},
                            {"argumentnames", arg_names.dump()},
                            {"argumentdefaults", "[\"\"]"},
                            {"warp", "false"}}}};
    work[def_id] = {{"opcode", "procedures_definition"},
                    {"next", body_id},
                    {"inputs", {{"custom_block", json::array({1, prototype_id})}}},
                    {"fields", json::object()},
                    {"shadow", false},
                    {"topLevel", true},
                    {"x", 0},
                    {"y", 0}};
    return "";
}