#include <nlohmann/json.hpp>

#include "compiler/ASTNode.hpp"
#include "compiler/BlockStatement.hpp"
using json = nlohmann::json;
json compile_project(TypeCheckerContext &ctx, BlockStatement &ast);
