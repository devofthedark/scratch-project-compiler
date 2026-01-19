#include "Statement.hpp"

StatementSubstitution Statement::make_statement_compat(const std::string &sprite_name,
                                                       const std::set<std::string> &args) {
    (void) args;
    (void) sprite_name;
    return {.new_statements = {}, .tmp_variables = 0, .replace_orig = false};
}