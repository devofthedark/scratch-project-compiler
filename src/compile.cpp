#include "compile.hpp"

#include <miniz.h>

#include <format>
#include <iostream>

#include "compiler/ASTNode.hpp"
#include "compiler/Lexer.hpp"
#include "compiler/Parser.hpp"
#include "exceptions/LanguageErrors.hpp"
#include "management/config.hpp"
#include "management/project.hpp"
#include "management/sprites.hpp"
#include "utility/file_utils.hpp"
#include "utility/md5.hpp"

namespace {
std::vector<uint8_t> read_file(const std::string &path) {
    std::ifstream file = file_utils::open_file(path, std::ios::binary);
    // NOLINTNEXTLINE
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                std::istreambuf_iterator<char>());
}
json compile_sprite(const std::string &name, mz_zip_archive *zipfile) {
    static TypeCheckerContext stage_ctx;
    std::filesystem::path sprite_dir = std::filesystem::current_path() / name;
    json result;
    std::map<std::string, CostumeConfig> costumes;
    std::string default_costume;
    if (name == ".") {
        json project_config_json;
        file_utils::open_file(sprite_dir / "config.json") >> project_config_json;
        ProjectConfig project_config = project_config_json.get<ProjectConfig>();
        result = json::object(
            {{"name", "Stage"},
             {"isStage", true},
             {"variables", json::object()},
             {"lists", json::object()},
             {"broadcasts", json::object()},
             {"blocks", json::object()},
             {"comments", json::object()},
             {"currentCostume", 0},
             {"costumes", json::array()},
             {"sounds", json::array()},
             // NOLINTNEXTLINE(readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
             {"volume", 100},
             {"layerOrder", 0}});
        costumes = project_config.costumes;
        default_costume = project_config.default_costume;
    } else {
        json sprite_config_json;
        file_utils::open_file(sprite_dir / "config.json") >> sprite_config_json;
        SpriteConfig sprite_config = sprite_config_json.get<SpriteConfig>();
        result = json::object({{"name", name},
                               {"variables", json::object()},
                               {"lists", json::object()},
                               {"broadcasts", json::object()},
                               {"blocks", json::object()},
                               {"comments", json::object()},
                               {"currentCostume", 0},
                               {"costumes", json::array()},
                               {"sounds", json::array()},
                               {"isStage", false},
                               {"volume", sprite_config.volume},
                               {"layerOrder", sprite_config.layer},
                               {"visible", sprite_config.visible},
                               {"x", sprite_config.starting_x},
                               {"y", sprite_config.starting_y},
                               {"size", sprite_config.size},
                               {"direction", sprite_config.starting_rotation},
                               {"draggable", sprite_config.draggable},
                               {"rotationStyle", sprite_config.rotation_style}});
        costumes = sprite_config.costumes;
        default_costume = sprite_config.default_costume;
    }
    // Load costumes
    std::map<std::string, size_t> costume_pos;
    for (const auto &[name, config] : costumes) {
        auto buffer = read_file((sprite_dir / "costumes" / (name + ".svg")).string());
        std::string md5sig =
            hashing::md5::sig2hex(hashing::md5::hash_bs(buffer.data(), buffer.size()));
        result["costumes"].push_back(json::object({{"assetId", md5sig},
                                                   {"name", name},
                                                   {"md5ext", md5sig + ".svg"},
                                                   {"dataFormat", "svg"},
                                                   {"rotationCenterX", config.pivot_x},
                                                   {"rotationCenterY", config.pivot_y}}));
        costume_pos[name] = result["costumes"].size() - 1;
        // Add to zipfile
        if (mz_zip_writer_add_mem(zipfile,
                                  (md5sig + ".svg").c_str(),
                                  buffer.data(),
                                  buffer.size(),
                                  MZ_BEST_COMPRESSION)
            == 0) {
            throw std::runtime_error(std::format("Could not add costume \"{}\" to sb3.", name));
        }
    }

    for (const auto &entry : std::filesystem::directory_iterator(sprite_dir / "costumes")) {
        if (entry.path().extension() != ".svg" || !entry.is_regular_file()) {
            continue;
        }
        std::string costume_name = entry.path().stem().string();
        if (!costume_pos.contains(costume_name)) {
            auto buffer = read_file(entry.path().string());
            std::string md5sig =
                hashing::md5::sig2hex(hashing::md5::hash_bs(buffer.data(), buffer.size()));
            result["costumes"].push_back(json::object({{"assetId", md5sig},
                                                       {"name", costume_name},
                                                       {"md5ext", md5sig + ".svg"},
                                                       {"dataFormat", "svg"},
                                                       {"rotationCenterX", 0},
                                                       {"rotationCenterY", 0}}));
            costume_pos[costume_name] = result["costumes"].size() - 1;
            if (mz_zip_writer_add_mem(zipfile,
                                      (md5sig + ".svg").c_str(),
                                      buffer.data(),
                                      buffer.size(),
                                      MZ_BEST_COMPRESSION)
                == 0) {
                throw std::runtime_error(
                    std::format("Could not add costume \"{}\" to sb3.", costume_name));
            }
        }
    }
    if (!costume_pos.contains(default_costume)) {
        throw std::runtime_error("Default costume not found in sprite: " + name);
    }
    result["currentCostume"] = costume_pos[default_costume];

    auto tokens = tokenize((sprite_dir / "script.spc").string());
    auto ast = parse_tokens(tokens.begin(), tokens.end());

    std::string main_script_id;
    auto cur_ctx = stage_ctx;
    try {
        auto ast_type = ast.typeCheck(cur_ctx);
        assert(ast_type != Type::ERROR);
        ast.make_statement_compat(name);
        ast.print();
        main_script_id = ast.compile(result["blocks"]);
    }

    catch (TypeError &e) {
        std::cerr << std::format("{}\nIn compiling of sprite \"{}\"\n",
                                 e.what(),
                                 name == "." ? "[default stage]" : name);
        throw std::runtime_error("");
    }
    // Add when flag clicked block
    result["blocks"]["init"] = json::object({{"opcode", "event_whenflagclicked"},
                                             {"next", main_script_id},
                                             {"inputs", json::object()},
                                             {"fields", json::object()},
                                             {"shadow", false},
                                             {"topLevel", true},
                                             {"x", 0},
                                             {"y", 0}});
    for (const auto &[name, type] : cur_ctx.getVariables()) {
        if (stage_ctx.lookupVariable(name) == Type::ERROR && type != Type::ERROR) {
            result["variables"][name] = json::array({name, 0});
        }
    }
    if (name == ".") {
        cur_ctx.reset_functions();
        stage_ctx = cur_ctx;
    }

    return result;
}
} // namespace
void compile() {
    if (!is_project()) {
        std::cerr << "Error: not in a project directory\n";
        std::exit(1);
    }
    std::filesystem::path current_dir = std::filesystem::current_path();
    // read project config
    json project_config_json;
    file_utils::open_file(current_dir / "config.json") >> project_config_json;
    ProjectConfig project_config = project_config_json.get<ProjectConfig>();
    json result = json::object({{"targets", json::array()},
                                {"monitors", json::array()},
                                {"extensions", json::array()},
                                {"meta",
                                 {{"semver", "3.0.0"},
                                  {"vm", "12.0.2-hotfix"},
                                  {"agent", "Scratch Project Compiler 0.1.0"}}}});

    // Create the sb3 zip archive
    mz_zip_archive zipfile;
    std::memset(&zipfile, 0, sizeof(zipfile));
    if (mz_zip_writer_init_file(&zipfile, "project.sb3", 0) == 0) {
        throw std::runtime_error("Failed to create sb3 archive");
    }
    try {
        // Compile stage
        result["targets"].push_back(compile_sprite(".", &zipfile));
        // Compile sprites
        for (const auto &sprite_name : project_config.sprites) {
            if (RESERVED_NAMES.contains(sprite_name)) {
                std::cerr << std::format(
                    "Warning: will not compile sprite with reserved name \"{}\"\n", sprite_name);
                continue;
            }
            result["targets"].push_back(compile_sprite(sprite_name, &zipfile));
        }
    } catch (TypeError &e) {
        std::cerr << e.what() << '\n';
    }
    // Write project.json to zipfile
    std::string project_json_str = result.dump(4);
    if (mz_zip_writer_add_mem(&zipfile,
                              "project.json",
                              project_json_str.data(),
                              project_json_str.size(),
                              MZ_BEST_COMPRESSION)
        == 0) {
        throw std::runtime_error("Could not add project.json to sb3.");
    }

    if (mz_zip_writer_finalize_archive(&zipfile) == 0) {
        throw std::runtime_error("Could not finalize project.sb3.");
    }
    mz_zip_writer_end(&zipfile);
}
