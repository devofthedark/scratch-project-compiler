#include "BlockStatement.hpp"

#include <format>
#include <iostream>

#include "compiler/ASTNode.hpp"
#include "compiler/Statement.hpp"

BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>> &_statements)
    : statements(std::make_move_iterator(_statements.begin()),
                 std::make_move_iterator(_statements.end())) {}
Type BlockStatement::typeCheck(TypeCheckerContext &ctx) const {
    for (const auto &stmt : statements) {
        if (stmt->typeCheck(ctx) == Type::ERROR) {
            return Type::ERROR;
        }
    }
    return Type::VOID;
}
void BlockStatement::add(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));
}
void BlockStatement::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "BlockStatement\n";
    int pos = 0;
    for (const auto &stmt : statements) {
        stmt->print(depth + 1, std::format("[{}]", pos));
        ++pos;
    }
}

StatementSubstitution BlockStatement::make_statement_compat(const std::set<std::string> &args) {
    StatementSubstitution return_value = {.new_statements = {},
                                          .tmp_variables = 0,
                                          .replace_orig = false};
    for (auto it = statements.begin(); it != statements.end();) {
        auto compat_statements = (*it)->make_statement_compat(args);
        statements.insert(it,
                          std::make_move_iterator(compat_statements.new_statements.begin()),
                          std::make_move_iterator(compat_statements.new_statements.end()));
        return_value.tmp_variables =
            std::max(compat_statements.tmp_variables, return_value.tmp_variables);
        if (compat_statements.replace_orig) {
            it = statements.erase(it);
        } else {
            ++it;
        }
    }
    return return_value;
}

std::string BlockStatement::compile(json &work) const {
    std::string last_id;
    for (const auto &statement : std::ranges::reverse_view(statements)) {
        std::string stmt_id = statement->compile(work);
        if (stmt_id.empty()) {
            continue;
        }
        if (!last_id.empty()) {
            work[stmt_id]["next"] = last_id;
        } else if (work[stmt_id]["next"].empty()) {
            work[stmt_id]["next"] = nullptr;
        }
        last_id = stmt_id;
    }
    return last_id;
}