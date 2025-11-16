#pragma once
#include <nlohmann/json.hpp>
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
    std::map<std::string, Type> variables;

public:
    TypeCheckerContext() = default;
    void addVariable(const std::string &name, Type type);
    void addFunction(const std::string &name, const std::vector<Type> &argTypes, Type returnType);
    [[nodiscard]] Type lookupVariable(const std::string &name) const;
    [[nodiscard]] const FunctionSignature *lookupFunction(const std::string &name,
                                                          const std::vector<Type> &argTypes) const;
    void removeVariable(const std::string &name);
    void setExpectedReturnType(Type type);
    [[nodiscard]] Type getExpectedReturnType() const;
    [[nodiscard]] const std::map<std::string, Type> &getVariables() const;
};
class ASTNode {
public:
    ASTNode() = default;
    ASTNode(const ASTNode &) = default;
    ASTNode(ASTNode &&) noexcept = default;
    ASTNode &operator=(const ASTNode &) = default;
    ASTNode &operator=(ASTNode &&) noexcept = default;
    virtual ~ASTNode() = default; // Virtual destructor
    virtual Type typeCheck(TypeCheckerContext &ctx) const;
    virtual void print(int depth = 0, std::string prefix = "");
    virtual std::string compile(json &work) const;
};

json num_value(std::string scratch_id);
inline std::string generate_id() {
    static int counter = 0;
    return "id_" + std::to_string(counter++);
}
constexpr std::string depth_prefix(int depth, std::string prefix) {
    if (depth == 0) {
        return prefix;
    }
    return std::string((size_t) (depth - 1) * 4, ' ') + "├── " + prefix;
}

template <typename T1, typename T2>
inline void replace_if_valid(std::unique_ptr<T1> &dest, std::unique_ptr<T2> src) {
    if (src) {
        dest = std::move(src);
    }
}