#include "VariableAssignment.hpp"
class VariableDeclaration : public VariableAssignment {
    private:
        Type type;

    public:
        VariableDeclaration(std::string name, std::unique_ptr<Expression> initializer, Type type);
        VariableDeclaration(VariableAssignment& assignment, Type type);
        Type typeCheck(TypeCheckerContext &ctx) const override;
        void print(int depth = 0, std::string prefix = "") override;
};