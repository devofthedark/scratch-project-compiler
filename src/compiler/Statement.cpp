#include "Statement.hpp"

StatementSubstitution Statement::make_statement_compat(const std::set<std::string> &args) {
    (void) args;
    return {.new_statements = {}, .tmp_variables = 0, .replace_orig = false};
}