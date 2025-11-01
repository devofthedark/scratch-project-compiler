#include "BlockStatement.hpp"
#include "compiler/ASTNode.hpp"

struct Parameter {
    std::string name;
    Type type;
};

class FunctionDeclaration : public Statement {
private:
    std::string name;
    std::unique_ptr<BlockStatement> body;
    std::vector<Parameter> parameters;
    Type returnType;

public:
    FunctionDeclaration(std::string name,
                        std::unique_ptr<BlockStatement> body,
                        std::vector<Parameter> parameters,
                        Type returnType);
    Type typeCheck(TypeCheckerContext &ctx) const override;
    [[nodiscard]] const std::string &getName() const;
    void print(int depth = 0, std::string prefix = "") override;
};