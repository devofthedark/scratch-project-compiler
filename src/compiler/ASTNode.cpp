#include "ASTNode.hpp"
void TypeCheckerContext::addVariable(const std::string& name, Type type) {
    variables[name] = type;
}
void TypeCheckerContext::addFunction(const std::string& name, const std::vector<Type>& argTypes, Type returnType) {
    FunctionKey key{name, argTypes};
    functions[key] = {argTypes, returnType};
}
Type TypeCheckerContext::lookupVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return Type::ERROR; // Variable not found
}
const FunctionSignature* TypeCheckerContext::lookupFunction(const std::string& name, const std::vector<Type>& argTypes) const {
    FunctionKey key{name, argTypes};
    auto it = functions.find(key);
    if (it != functions.end()) {
        return &it->second;
    }
    return nullptr; // Function not found
}
void TypeCheckerContext::removeVariable(const std::string& name) {
    variables.erase(name);
}
void TypeCheckerContext::setExpectedReturnType(Type type) {
    expectedReturnType = type;
}
Type TypeCheckerContext::getExpectedReturnType() const {
    return expectedReturnType;
}
std::string ASTNode::depth_prefix(int depth, std::string prefix) const {
    if (depth == 0) {
        return prefix;
    }
    return std::string((size_t)(depth-1)*4, ' ') + "├── " + prefix;
}
std::string ASTNode::generate_id() const{
    static int counter = 0;
    return "id_" + std::to_string(counter++);
}
std::string ASTNode::compile(json &work) const {
    (void)work; // suppress unused parameter warning 
    throw std::runtime_error("compile() not implemented for this ASTNode");
}