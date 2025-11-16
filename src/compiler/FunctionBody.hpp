#pragma once
#include "BlockStatement.hpp"

class FunctionBody : public BlockStatement {
public:
    explicit FunctionBody(std::vector<std::unique_ptr<Statement>> &statements);
    explicit FunctionBody(BlockStatement &&to_conv);
};