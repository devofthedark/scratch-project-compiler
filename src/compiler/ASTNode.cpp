#include "ASTNode.hpp"

#include <iostream>
#include <utility>
void TypeCheckerContext::addVariable(const std::string &name, Type type) {
    variables[name] = type;
}
void TypeCheckerContext::addFunction(const std::string &name,
                                     const std::vector<Type> &argTypes,
                                     Type returnType,
                                     std::shared_ptr<BlockStatement> implementation,
                                     bool is_stdcall) {
    FunctionKey key{.name = name, .argTypes = argTypes};
    functions[key] = {.argTypes = argTypes,
                      .returnType = returnType,
                      .implementation = std::move(implementation),
                      .is_stdcall = is_stdcall};
}
Type TypeCheckerContext::lookupVariable(const std::string &name) const {
    auto itr = variables.find(name);
    if (itr != variables.end()) {
        return itr->second;
    }
    return Type::ERROR; // Variable not found
}
const FunctionSignature *TypeCheckerContext::lookupFunction(
    const std::string &name, const std::vector<Type> &argTypes) const {
    FunctionKey key{.name = name, .argTypes = argTypes};
    auto itr = functions.find(key);
    if (itr != functions.end()) {
        return &itr->second;
    }
    return nullptr; // Function not foundi
}
void TypeCheckerContext::removeVariable(const std::string &name) {
    variables.erase(name);
}
void TypeCheckerContext::setExpectedReturnType(Type type) {
    expectedReturnType = type;
}
Type TypeCheckerContext::getExpectedReturnType() const {
    return expectedReturnType;
}
const std::map<std::string, Type> &TypeCheckerContext::getVariables() const {
    return variables;
}

void ASTNode::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, std::move(prefix)) << "Unknown ASTNode\n";
}
std::string ASTNode::compile(json &work) const {
    (void) work; // suppress unused parameter warning
    throw std::runtime_error("compile() not implemented for this ASTNode");
}

json num_value(std::string scratch_id) {
    if (scratch_id[0] == '[' && scratch_id[1] == '4') {
        return json::array({1, json::parse(scratch_id)});
    }
    if (scratch_id[0] == '[' && scratch_id[1] == '1') {
        return json::array({3, json::parse(scratch_id), json::array({4, "0"})});
    }
    return json::array({3, scratch_id, json::array({4, "0"})});
}

Type ASTNode::typeCheck(TypeCheckerContext &ctx) {
    (void) ctx;
    throw std::runtime_error("what");
}
