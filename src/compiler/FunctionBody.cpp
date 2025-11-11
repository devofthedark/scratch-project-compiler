#include "FunctionBody.hpp"

#include <memory>

#include "compiler/BlockStatement.hpp"
#include "compiler/Statement.hpp"

FunctionBody::FunctionBody(std::vector<std::unique_ptr<Statement>> &statements)
    : BlockStatement(statements) {}
FunctionBody::FunctionBody(BlockStatement &&to_conv) : BlockStatement(std::move(to_conv)) {}
