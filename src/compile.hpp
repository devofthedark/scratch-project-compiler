#include <nlohmann/json.hpp>
#include "compiler/BlockStatement.hpp"
#include "compiler/ASTNode.hpp"
using json = nlohmann::json;
json compile_project(TypeCheckerContext &ctx, const BlockStatement &ast);
