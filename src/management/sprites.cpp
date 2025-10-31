#include <fmt/ostream.h>
#include <fstream>
#include <nlohmann/json.hpp>

#include "config.hpp"
#include "strings.hpp"
using json = nlohmann::json;
void new_sprite(std::string name) {
    std::filesystem::path current_dir = std::filesystem::current_path();
    // read config.json into a Projectconfig object
    std::ifstream ifs(current_dir / "config.json");
    if (!ifs) {
        fmt::println(stderr, "Error: cannot open config.json");
        std::exit(1);
    }
    json j;
    ifs >> j;
    ProjectConfig config = j.get<ProjectConfig>();
    ifs.close();
    // check if the sprite already exists
    for (const auto &sprite : config.sprites) {
        if (sprite == name) {
            fmt::println(stderr, strings::errors::sprite_exists, name);
            std::exit(1);
        }
    }
    // create the sprite directory
    std::filesystem::create_directory(current_dir / name);
    // create the costume directory
    std::filesystem::create_directory(current_dir / name / "costumes");
    // create the sounds directory
    std::filesystem::create_directory(current_dir / name / "sounds");
    // create the script.scratch file
    std::ofstream ofs(current_dir / name / "script.scratch");
    if (!ofs) {
        fmt::println(stderr, "Error: cannot create script.scratch");
        std::exit(1);
    }
    ofs.close();
    // create the config.json file
    std::ofstream ofs2(current_dir / name / "config.json");
    if (!ofs2) {
        fmt::println(stderr, "Error: cannot create config.json");
        std::exit(1);
    }
    ofs2 << std::setw(4) << json(SpriteConfig(name)) << std::endl;
    ofs2.close();
    // add the sprite to the project config
    config.sprites.push_back(name);
    // write the project config back to the file
    std::ofstream ofs3(current_dir / "config.json");
    if (!ofs3) {
        fmt::println(stderr, "Error: cannot open config.json");
        std::exit(1);
    }
    ofs3 << std::setw(4) << json(config) << std::endl;
    ofs3.close();
    fmt::println("Sprite {} created", name);
}
void delete_sprite(std::string name) {
    std::filesystem::path current_dir = std::filesystem::current_path();
    // read config.json into a Projectconfig object
    std::ifstream ifs(current_dir / "config.json");
    if (!ifs) {
        fmt::println(stderr, "Error: cannot open config.json");
        std::exit(1);
    }
    json j;
    ifs >> j;
    ProjectConfig config = j.get<ProjectConfig>();
    ifs.close();
    // check if the sprite exists
    bool found = false;
    for (const auto &sprite : config.sprites) {
        if (sprite == name) {
            found = true;
            break;
        }
    }
    if (!found) {
        fmt::println(stderr, strings::errors::sprite_does_not_exist, name);
        std::exit(1);
    }
    // remove the sprite from the project config
    config.sprites.erase(
        std::remove(config.sprites.begin(), config.sprites.end(), name),
        config.sprites.end());
    // write the project config back to the file
    std::ofstream ofs(current_dir / "config.json");
    if (!ofs) {
        fmt::println(stderr, "Error: cannot open config.json");
        std::exit(1);
    }
    ofs << std::setw(4) << json(config) << std::endl;
    ofs.close();

    // remove the sprite directory
    std::filesystem::remove_all(current_dir / name);
    fmt::println("Sprite {} deleted", name);
}