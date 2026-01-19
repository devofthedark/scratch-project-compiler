#pragma once
#include <nlohmann/json.hpp>
// The following are basically structs for serializing and deserializing JSON
// NOLINTBEGIN(misc-non-private-member-variables-in-classes)
struct CostumeConfig {
    double pivot_x = 0;
    double pivot_y = 0;
    CostumeConfig() = default;
};
struct SpriteConfig {
    std::string name;
    std::string default_costume;
    double starting_x = 0;
    double starting_y = 0;
    // NOLINTNEXTLINE(readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
    double starting_rotation = 90;
    double starting_scale = 1;
    int layer = 1;
    // NOLINTBEGIN(readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
    double volume = 100;
    double size = 100;
    // NOLINTEND(readability-magic-numbers, cppcoreguidelines-avoid-magic-numbers)
    bool visible = true;
    bool draggable = false;
    std::string rotation_style = "all around";
    std::map<std::string, CostumeConfig> costumes;
    explicit SpriteConfig(std::string _name) : name(std::move(_name)) {}
    SpriteConfig() = default;
};
struct ProjectConfig {
    std::vector<std::string> sprites;
    std::map<std::string, CostumeConfig> costumes;
    std::string default_costume;
    ProjectConfig() : sprites({}) {}
};
// NOLINTEND(misc-non-private-member-variables-in-classes)

// nlohmann json serialization dosent use constraints, so disable the linter for this section
// NOLINTBEGIN(modernize-use-constraints)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CostumeConfig, pivot_x, pivot_y);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpriteConfig,
                                   name,
                                   default_costume,
                                   starting_x,
                                   starting_y,
                                   starting_rotation,
                                   starting_scale,
                                   layer,
                                   volume,
                                   size,
                                   visible,
                                   draggable,
                                   rotation_style,
                                   costumes);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ProjectConfig, sprites, costumes, default_costume);
// NOLINTEND(modernize-use-constraints)