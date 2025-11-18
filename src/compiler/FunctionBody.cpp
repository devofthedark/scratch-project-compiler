#include "FunctionBody.hpp"

FunctionBody::FunctionBody(std::vector<std::unique_ptr<Statement>> &statements)
    : BlockStatement(statements) {}
FunctionBody::FunctionBody(BlockStatement &&to_conv) : BlockStatement(std::move(to_conv)) {}
