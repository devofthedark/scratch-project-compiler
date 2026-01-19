#include "BlockStatement.hpp"

#include <format>
#include <iostream>

#include "compiler/ASTNode.hpp"
#include "compiler/Statement.hpp"

BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>> &_statements)
    : statements(std::make_move_iterator(_statements.begin()),
                 std::make_move_iterator(_statements.end())) {}
Type BlockStatement::typeCheck(TypeCheckerContext &ctx) {
    for (const auto &stmt : statements) {
        auto res = stmt->typeCheck(ctx);
        assert(res != Type::ERROR);
    }
    return Type::VOID;
}

void BlockStatement::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "BlockStatement\n";
    int pos = 0;
    for (const auto &stmt : statements) {
        stmt->print(depth + 1, std::format("[{}]", pos));
        ++pos;
    }
}

StatementSubstitution BlockStatement::make_statement_compat(const std::string &sprite_name,
                                                            const std::set<std::string> &args) {
    StatementSubstitution return_value = {.new_statements = {},
                                          .tmp_variables = 0,
                                          .replace_orig = false};
    for (auto it = statements.begin(); it != statements.end();) {
        auto compat_statements = (*it)->make_statement_compat(sprite_name, args);
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
        if (!statement) {
            continue;
        }
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