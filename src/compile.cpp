#include "compile.hpp"

#include <format>
#include <fstream>
#include <iostream>
json compile_project(TypeCheckerContext &ctx, BlockStatement &ast) {
    // Temporary hardcode a lot of stuff
    // NOLINTBEGIN
    json result = {
        {"targets",
         json::array({{{"isStage", true},
                       {"name", "Stage"},
                       {"variables", json::object()},
                       {"lists", json::object()},
                       {"broadcasts", json::object()},
                       {"blocks", json::object()},
                       {"comments", json::object()},
                       {"currentCostume", 0},
                       {"costumes",
                        {{{"assetId", "cd21514d0531fdffb22204e0ec5ed84a"},
                          {"name", "backdrop1"},
                          {"md5ext", "cd21514d0531fdffb22204e0ec5ed84a.svg"},
                          {"dataFormat", "svg"},
                          {"rotationCenterX", 240},
                          {"rotationCenterY", 180}}}},
                       {"sounds", json::array()},
                       {"volume", 100},
                       {"layerOrder", 0}},
                      {{"isStage", false},
                       {"name", "Sprite1"},
                       {"variables", json::object()},
                       {"lists", json::object()},
                       {"broadcasts", json::object()},
                       {"blocks", json::object()},
                       {"comments", json::object()},
                       {"currentCostume", 0},
                       {"costumes",
                        {{{"assetId", "bcf454acf82e4504149f7ffe07081dbc"},
                          {"name", "costume1"},
                          {"md5ext", "bcf454acf82e4504149f7ffe07081dbc.svg"},
                          {"dataFormat", "svg"},
                          {"rotationCenterX", 48},
                          {"rotationCenterY", 50}}}},
                       {"sounds", json::array()},
                       {"volume", 100},
                       {"layerOrder", 1},
                       {"visible", true},
                       {"x", 0},
                       {"y", 0},
                       {"size", 100},
                       {"direction", 90},
                       {"draggable", false},
                       {"rotationStyle", "all around"}}})},
        {"monitors", json::array()},
        {"extensions", json::array()},
        {"meta",
         {{"semver", "3.0.0"}, {"vm", "12.0.2-hotfix"}, {"agent", "Scratch Compiler 0.1.0"}}}};
    // NOLINTEND
    Type ast_type = ast.typeCheck(ctx);
    if (ast_type == Type::ERROR) {
        throw std::runtime_error("Type checking failed");
    }
    int n_tmp = ast.make_statement_compat().tmp_variables;
    for (const auto &var : std::views::keys(ctx.getVariables())) {
        result["targets"][1]["variables"][var] = json::array({var, 0});
        std::cerr << "Added variable to JSON: " << var << '\n';
    }
    for (int i = 0; i < n_tmp; ++i) {
        std::string tmp_var_name = std::format("__scratch_compiler_internal_tmp_var_{}", i);
        result["targets"][1]["variables"][tmp_var_name] = json::array({tmp_var_name, 0});
        std::cerr << "Added variable to JSON: " << tmp_var_name << '\n';
    }
    std::string main_script_id = ast.compile(result["targets"][1]["blocks"]);
    // Add when flag clicked block
    result["targets"][1]["blocks"]["init"] = {{"opcode", "event_whenflagclicked"},
                                              {"next", main_script_id},
                                              {"inputs", json::object()},
                                              {"fields", json::object()},
                                              {"shadow", false},
                                              {"topLevel", true},
                                              {"x", 0},
                                              {"y", 0}};
    std::ofstream ofs("out.json");
    ofs << std::setw(4) << result << '\n';
    ofs.close();
    return result;
}