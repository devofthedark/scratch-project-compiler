#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
class SpriteConfig {
    public:
        std::string name;
        std::string default_costume;
        double starting_x;
        double starting_y;
        double starting_rotation;
        double starting_scale;
        SpriteConfig(std::string _name) : name(_name), default_costume(""), starting_x(0), starting_y(0), starting_rotation(0), starting_scale(1) {}   
};
class ProjectConfig {
    public:
        std::vector<std::string> sprites;
        std::string default_costume;
        ProjectConfig() : sprites({}), default_costume("") {}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpriteConfig, name, default_costume, starting_x, starting_y, starting_rotation, starting_scale);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectConfig, sprites, default_costume);