#include "FunctionDeclaration.hpp"

#include <format>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

#include "compiler/FunctionArgument.hpp"
#include "compiler/FunctionBody.hpp"
#include "exceptions/LanguageErrors.hpp"

FunctionDeclaration::FunctionDeclaration(std::string _name,
                                         std::shared_ptr<BlockStatement> _body,
                                         std::vector<Parameter> _parameters,
                                         Type _returnType,
                                         bool is_stdcall)
    : name(std::move(_name)), body(std::move(_body)), parameters(std::move(_parameters)),
      returnType(_returnType), is_stdcall(is_stdcall) {}
Type FunctionDeclaration::typeCheck(TypeCheckerContext &ctx) {
    // Check the return type
    assert(returnType != Type::ERROR);
    // Check if the function name is already used
    // Check if name starts with "__"
    if (name.starts_with("__")) {
        throw TypeError(std::format(
            R"(Function "{}" violates reserved name rule: function names starting with "__" are reserved.)",
            name));
    }

    // Check the parameters
    std::vector<Type> param_types;
    std::map<std::string, Type> old_types;
    for (const auto &param : parameters) {
        assert(param.type != Type::ERROR);
        old_types[param.name] = ctx.lookupVariable(param.name);
        // Add the parameter to the context
        ctx.addVariable(param.name, param.type);
        param_types.push_back(param.type);
    }
    if (ctx.lookupFunction(name, param_types) != nullptr) {
        throw TypeError(std::format("Cannot declare {}: function already exists.",
                                    func_sig_str(name, param_types)));
    }
    // Set return type
    ctx.setExpectedReturnType(returnType);
    // Add the function to the context
    ctx.addFunction(name, param_types, returnType, body, is_stdcall);

    // Check the function body

    Type tmp = Type::ERROR;
    try {
        tmp = body->typeCheck(ctx);
    } catch (TypeError &e) {
        std::cerr << e.what() << '\n';
        if (returnType != Type::VOID) {
            throw TypeError(std::format("In definition of {} -> {}",
                                        func_sig_str(name, param_types),
                                        type_str(returnType)));
        }
        throw TypeError(std::format("In definition of {}", func_sig_str(name, param_types)));
    }
    assert(tmp != Type::ERROR);
    ctx.setExpectedReturnType(Type::VOID);
    for (const auto &[name, type] : old_types) {
        ctx.addVariable(name, type); // If type is Type::ERROR than this is hacky af
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
    const std::string &sprite_name, const std::set<std::string> &args) {
    (void) args;
    std::set<std::string> rep;
    for (const auto &param : parameters) {
        rep.insert(param.name);
    }
    auto ret_val = body->make_statement_compat(sprite_name, rep);
    if (is_stdcall) {
        return ret_val;
    }
    body = std::make_unique<FunctionBody>(std::move(*body));
    return ret_val;
}

std::string FunctionDeclaration::compile(json &work) const {
    if (is_stdcall) {
        return "";
    }
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
    work[prototype_id] = json::object({{"opcode", "procedures_prototype"},
                                       {"next", nullptr},
                                       {"inputs", inputs},
                                       {"fields", json::object()},
                                       {"shadow", true},
                                       {"topLevel", false},
                                       {"mutation",
                                        json::object({{"tagName", "mutation"},
                                                      {"children", json::array()},
                                                      {"proccode", proccode},
                                                      {"argumentids", argids.dump()},
                                                      {"argumentnames", arg_names.dump()},
                                                      {"argumentdefaults", "[\"\"]"},
                                                      {"warp", "false"}})}});
    work[def_id] =
        json::object({{"opcode", "procedures_definition"},
                      {"next", body_id},
                      {"inputs", json::object({{"custom_block", json::array({1, prototype_id})}})},
                      {"fields", json::object()},
                      {"shadow", false},
                      {"topLevel", true},
                      {"x", 0},
                      {"y", 0}});
    return "";
}