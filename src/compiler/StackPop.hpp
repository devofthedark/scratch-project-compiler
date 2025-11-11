#include "Statement.hpp"

class StackPop : public Statement {
public:
    void print(int depth = 0, std::string prefix = "") override;
    std::string compile(json &work) const override;
};