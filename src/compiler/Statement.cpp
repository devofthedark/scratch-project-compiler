#include "Statement.hpp"

StatementSubstitution Statement::make_statement_compat() {
    return {.new_statements = {}, .tmp_variables = 0, .replace_orig = false};
}