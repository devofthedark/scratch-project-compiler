#include "compile.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "management/config.hpp"
#include "utility/file_utils.hpp"
#include "utility/md5.hpp"

namespace {
std::vector<uint8_t> read_file(const std::string &path) {
    std::ifstream file = file_utils::open_file(path, std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                std::istreambuf_iterator<char>());
}
} // namespace
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
    ast.print();
    for (int i = 0; i < n_tmp; ++i) {
        std::string tmp_var_name = std::format("__scratch_compiler_tmp_var_{}", i);
        result["targets"][1]["variables"][tmp_var_name] = json::array({tmp_var_name, 0});
        std::cerr << "Added variable to JSON: " << tmp_var_name << '\n';
    }
    result["targets"][1]["variables"]["return_val"] =
        json::array({"__scratch_compiler_function_return_value", 0});

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

json compile_sprite(const std::string &name) {
    std::filesystem::path sprite_dir = std::filesystem::current_path() / name;
    json sprite_config_json;
    file_utils::open_file(sprite_dir / "config.json") >> sprite_config_json;
    SpriteConfig sprite_config = sprite_config_json.get<SpriteConfig>();
    json result = {{"name", name},
                   {"variables", json::object()},
                   {"lists", json::object()},
                   {"broadcasts", json::object()},
                   {"blocks", json::object()},
                   {"comments", json::object()},
                   {"currentCostume", 0},
                   {"costumes", json::array()},
                   {"sounds", json::array()},
                   {"volume", sprite_config.volume},
                   {"layerOrder", sprite_config.layer},
                   {"visible", sprite_config.visible},
                   {"x", sprite_config.starting_x},
                   {"y", sprite_config.starting_y},
                   {"size", sprite_config.size},
                   {"direction", sprite_config.starting_rotation},
                   {"draggable", sprite_config.draggable},
                   {"rotationStyle", sprite_config.rotation_style}};
    std::map<std::string, size_t> costume_pos;
    for (const auto &[name, config] : sprite_config.costumes) {
        auto buffer = read_file((sprite_dir / "costumes" / (name + ".svg")).string());
        std::string md5sig =
            hashing::md5::sig2hex(hashing::md5::hash_bs(buffer.data(), buffer.size()));
        result["costumes"].push_back({{"assetId", md5sig},
                                      {"name", name},
                                      {"md5ext", md5sig + ".svg"},
                                      {"dataFormat", "svg"},
                                      {"rotationCenterX", config.pivot_x},
                                      {"rotationCenterY", config.pivot_y}});
        costume_pos[name] = result["costumes"].size() - 1;
    }
    for (const auto &entry : std::filesystem::directory_iterator(sprite_dir / "scripts")) {
        if (entry.path().extension() != ".svg" || !entry.is_regular_file()) {
            continue;
        }
        std::string costume_name = entry.path().stem().string();
        auto buffer = read_file(entry.path().string());
        std::string md5sig =
            hashing::md5::sig2hex(hashing::md5::hash_bs(buffer.data(), buffer.size()));
        if (!costume_pos.contains(costume_name)) {
            result["costumes"].push_back({{"assetId", md5sig},
                                          {"name", costume_name},
                                          {"md5ext", md5sig + ".svg"},
                                          {"dataFormat", "svg"},
                                          {"rotationCenterX", 0},
                                          {"rotationCenterY", 0}});
        }
        costume_pos[costume_name] = result["costumes"].size() - 1;
    }
    if (!costume_pos.contains(sprite_config.default_costume)) {
        throw std::runtime_error("Default costume not found in sprite: " + name);
    }
    result["currentCostume"] = costume_pos[sprite_config.default_costume];

    return result;
}