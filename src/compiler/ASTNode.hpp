#pragma once
#include <vector>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
enum class Type {
    DOUBLE,
    BOOL,
    STRING,
    VOID, // Used for functions that don't return anything
    ERROR // Not *really* a type, but used for error handling
};
struct FunctionSignature {
    std::vector<Type> argTypes;
    Type returnType;
};
struct FunctionKey {
    std::string name;
    std::vector<Type> argTypes;
    bool operator==(const FunctionKey& other) const {
        return name == other.name && argTypes == other.argTypes;
    }
    bool operator<(const FunctionKey& other) const {
        if (name != other.name) return name < other.name;
        return argTypes < other.argTypes;
    }
};
class TypeCheckerContext {
    private:
        std::map<FunctionKey, FunctionSignature> functions;
        Type expectedReturnType = Type::VOID;
    public:
        std::map<std::string, Type> variables;
        TypeCheckerContext() = default; 
        void addVariable(const std::string& name, Type type);
        void addFunction(const std::string& name, const std::vector<Type>& argTypes, Type returnType);
        Type lookupVariable(const std::string& name) const;
        const FunctionSignature* lookupFunction(const std::string& name, const std::vector<Type>& argTypes) const;
        void removeVariable(const std::string& name);
        void setExpectedReturnType(Type type);
        Type getExpectedReturnType() const;
};
class ASTNode {
    protected:
        std::string generate_id() const;
        std::string depth_prefix(int depth, std::string prefix = "") const;
    public:
        virtual ~ASTNode() = default;
        virtual Type typeCheck(TypeCheckerContext &ctx) const = 0;
        virtual void print(int depth = 0, std::string prefix = "") = 0;
        virtual std::string compile(json &work) const;
};