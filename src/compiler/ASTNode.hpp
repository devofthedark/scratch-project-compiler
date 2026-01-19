#pragma once
#include <format>
#include <nlohmann/json.hpp>

#include "exceptions/LanguageErrors.hpp"
using json = nlohmann::json;
enum class Type : uint8_t {
    DOUBLE,
    BOOL,
    STRING,
    VOID, // Used for functions that don't return anything
    ERROR // Not *really* a type, but used for error handling
};

inline std::string type_str(Type type) {
    switch (type) {
        case Type::DOUBLE:
            return "num";
        case Type::BOOL:
            return "bool";
        case Type::STRING:
            return "str";
        case Type::VOID:
        case Type::ERROR:
            break;
    }
    throw std::runtime_error("not implemented");
}

class BlockStatement;
class Expression;
struct FunctionSignature {
    std::vector<Type> argTypes;
    Type returnType;
    std::shared_ptr<BlockStatement> implementation;
    bool is_stdcall;
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
inline std::string func_sig_str(const std::string &name, const std::vector<Type> &param_types) {
    std::string ret_val = std::format("fn {}(", name);
    if (param_types.empty()) {
        return ret_val + ")";
    }
    for (const auto &type : param_types) {
        assert(type != Type::VOID || type != Type::ERROR);
        ret_val += type_str(type) + ", ";
    }
    return ret_val.substr(0, ret_val.length() - 2) + ")";
}
inline std::string func_sig_str(const FunctionKey &key) {
    return func_sig_str(key.name, key.argTypes);
}
class TypeCheckerContext {
private:
    std::map<FunctionKey, FunctionSignature> functions;
    Type expectedReturnType = Type::VOID;
    std::map<std::string, Type> variables;

public:
    TypeCheckerContext() = default;
    void addVariable(const std::string &name, Type type);
    void addFunction(const std::string &name,
                     const std::vector<Type> &argTypes,
                     Type returnType,
                     std::shared_ptr<BlockStatement> implementation,
                     bool is_stdcall = false);
    [[nodiscard]] Type lookupVariable(const std::string &name) const;
    [[nodiscard]] const FunctionSignature *lookupFunction(const std::string &name,
                                                          const std::vector<Type> &argTypes) const;
    void removeVariable(const std::string &name);
    void setExpectedReturnType(Type type);
    [[nodiscard]] Type getExpectedReturnType() const;
    [[nodiscard]] const std::map<std::string, Type> &getVariables() const;
    void reset_functions();
};

class ASTNode {
public:
    ASTNode() = default;
    ASTNode(const ASTNode &) = default;
    ASTNode(ASTNode &&) noexcept = default;
    ASTNode &operator=(const ASTNode &) = default;
    ASTNode &operator=(ASTNode &&) noexcept = default;
    virtual ~ASTNode() = default; // Virtual destructor
    virtual Type typeCheck(TypeCheckerContext &ctx);
    virtual void print(int depth = 0, std::string prefix = "");
    virtual std::string compile(json &work) const;
    [[nodiscard]] virtual bool is_stdcall_hook() const {
        return false;
    }
    virtual void add_arg_to_stdcall(const std::string &arg) {
        (void) arg;
        throw TypeError("stdcall function has been defined improperly");
    }
    virtual void clear_stdcall_args() {
        throw TypeError("stdcall function has been defined improperly");
    }
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