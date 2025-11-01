#pragma once
#include <sys/types.h>

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
using json = nlohmann::json;
enum class Type : uint8_t {
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
// Just a data struct
// NOLINTBEGIN(misc-non-private-member-variables-in-classes)
struct FunctionKey {
    std::string name;
    std::vector<Type> argTypes;
    bool operator==(const FunctionKey &other) const {
        return name == other.name && argTypes == other.argTypes;
    }
    bool operator<(const FunctionKey &other) const {
        if (name != other.name) {
            return name < other.name;
        }
        return argTypes < other.argTypes;
    }
};
// NOLINTEND(misc-non-private-member-variables-in-classes)
class TypeCheckerContext {
private:
    std::map<FunctionKey, FunctionSignature> functions;
    Type expectedReturnType = Type::VOID;

public:
    // TODO: place this in private
    // NOLINTNEXTLINE
    std::map<std::string, Type> variables;
    TypeCheckerContext() = default;
    void addVariable(const std::string &name, Type type);
    void addFunction(const std::string &name, const std::vector<Type> &argTypes, Type returnType);
    [[nodiscard]] Type lookupVariable(const std::string &name) const;
    [[nodiscard]] const FunctionSignature *lookupFunction(const std::string &name,
                                                          const std::vector<Type> &argTypes) const;
    void removeVariable(const std::string &name);
    void setExpectedReturnType(Type type);
    [[nodiscard]] Type getExpectedReturnType() const;
};
class ASTNode {
protected:
    [[nodiscard]] static std::string generate_id();
    [[nodiscard]] static std::string depth_prefix(int depth, std::string prefix = "");

public:
    ASTNode() = default;
    ASTNode(const ASTNode &) = default;
    ASTNode(ASTNode &&) noexcept = default;
    ASTNode &operator=(const ASTNode &) = default;
    ASTNode &operator=(ASTNode &&) noexcept = default;
    virtual ~ASTNode() = default; // Virtual destructor
    virtual Type typeCheck(TypeCheckerContext &ctx) const = 0;
    virtual void print(int depth = 0, std::string prefix = "") = 0;
    virtual std::string compile(json &work) const;
};