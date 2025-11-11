#pragma once
#include <memory>

#include "BlockStatement.hpp"
#include "compiler/Statement.hpp"

class FunctionBody : public BlockStatement {
public:
    explicit FunctionBody(std::vector<std::unique_ptr<Statement>> &statements);
    explicit FunctionBody(BlockStatement &&to_conv);
};