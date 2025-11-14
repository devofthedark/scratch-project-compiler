#include <nlohmann/json.hpp>
#include <string>

#include "compiler/ASTNode.hpp"
#include "compiler/BlockStatement.hpp"
using json = nlohmann::json;
json compile_project(TypeCheckerContext &ctx, BlockStatement &ast);
json compile_sprite(const std::string &name);
void compile();
