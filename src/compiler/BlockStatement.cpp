#include "BlockStatement.hpp"

#include <iostream>
#include <ranges>

#include "compiler/ASTNode.hpp"

BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>> &_statements)
    : statements(std::move(_statements)) {}
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
    for (size_t i = 0; i < statements.size(); i++) {
        statements[i]->print(depth + 1, "[" + std::to_string(i) + "] ");
    }
}
std::string BlockStatement::compile(json &work) const {
    std::string last_id;
    for (const auto & statement : std::ranges::reverse_view(statements)) {
        std::string stmt_id = statement->compile(work);
        if (!last_id.empty()) {
            work[stmt_id]["next"] = last_id;
        } else {
            work[stmt_id]["next"] = nullptr;
        }
        last_id = stmt_id;
    }
    return last_id;
}