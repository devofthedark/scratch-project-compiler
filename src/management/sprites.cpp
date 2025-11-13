#include "sprites.hpp"

#include <format>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "config.hpp"
#include "strings.hpp"
using json = nlohmann::json;
void new_sprite(std::string name) {
    std::filesystem::path current_dir = std::filesystem::current_path();
    // read config.json into a Projectconfig object
    std::ifstream ifs(current_dir / "config.json");
    json json_data;
    ifs >> json_data;
    ProjectConfig config = json_data.get<ProjectConfig>();
    ifs.close();
    // check if the sprite already exists
    for (const auto &sprite : config.sprites) {
        if (sprite == name) {
            std::cerr << std::format(strings::errors::SPRITE_EXISTS, name) << '\n';
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
    std::ofstream ofs(current_dir / name / "script.spc");
    ofs.close();
    // create the config.json file
    std::ofstream ofs2(current_dir / name / "config.json");
    ofs2 << std::setw(4) << json(SpriteConfig(name)) << '\n';
    ofs2.close();
    // add the sprite to the project config
    config.sprites.push_back(name);
    // write the project config back to the file
    std::ofstream ofs3(current_dir / "config.json");
    ofs3 << std::setw(4) << json(config) << '\n';
    ofs3.close();
    std::cout << std::format("Sprite {} created\n", name);
}
void delete_sprite(std::string name) {
    std::filesystem::path current_dir = std::filesystem::current_path();
    // read config.json into a Projectconfig object
    std::ifstream ifs(current_dir / "config.json");
    json json_data;
    ifs >> json_data;
    ProjectConfig config = json_data.get<ProjectConfig>();
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
        std::cerr << std::format(strings::errors::SPRITE_DOES_NOT_EXIST, name) << '\n';
        std::exit(1);
    }
    // remove the sprite from the project config
    auto itr = std::ranges::remove(config.sprites, name);
    config.sprites.erase(itr.begin(), itr.end());

    // write the project config back to the file
    std::ofstream ofs(current_dir / "config.json");
    ofs << std::setw(4) << json(config) << '\n';
    ofs.close();

    // remove the sprite directory
    std::filesystem::remove_all(current_dir / name);
    std::cout << std::format("Sprite {} deleted\n", name);
}