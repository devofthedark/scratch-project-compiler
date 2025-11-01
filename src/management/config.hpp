#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>
// The following are basically structs for serializing and deserializing JSON
// NOLINTBEGIN(misc-non-private-member-variables-in-classes)
class SpriteConfig {
public:
    std::string name;
    std::string default_costume;
    double starting_x = 0;
    double starting_y = 0;
    double starting_rotation = 0;
    double starting_scale = 1;
    explicit SpriteConfig(std::string _name) : name(std::move(_name)) {}
};
class ProjectConfig {
public:
    std::vector<std::string> sprites;
    std::string default_costume;
    ProjectConfig() : sprites({}) {}
};
// NOLINTEND(misc-non-private-member-variables-in-classes)

// nlohmann json serialization dosent use constraints, so disable the linter for this section
// NOLINTBEGIN(modernize-use-constraints)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    SpriteConfig, name, default_costume, starting_x, starting_y, starting_rotation, starting_scale);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectConfig, sprites, default_costume);
// NOLINTEND(modernize-use-constraints)