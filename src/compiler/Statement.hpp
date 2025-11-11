#pragma once
#include <set>

#include "ASTNode.hpp"

class Statement;

struct StatementSubstitution {
    std::vector<std::unique_ptr<Statement>> new_statements;
    int tmp_variables;
    bool replace_orig;
};

class Statement : virtual public ASTNode {
public:
    virtual StatementSubstitution make_statement_compat(const std::set<std::string> &args = {});
};