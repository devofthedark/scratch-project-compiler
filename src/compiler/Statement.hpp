#pragma once
#include "ASTNode.hpp"

class Statement;

struct StatementSubstitution {
    std::vector<std::unique_ptr<Statement>> compat_statements;
    int num_new_variables;
    bool replace_original;
};

class Statement : virtual public ASTNode {
public:
    virtual StatementSubstitution make_statement_compat();
};