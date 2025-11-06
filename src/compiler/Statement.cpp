#include "Statement.hpp"

StatementSubstitution Statement::make_statement_compat() {
    return {.compat_statements = {}, .num_new_variables = 0, .replace_original = false};
}