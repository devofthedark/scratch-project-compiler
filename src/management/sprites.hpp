#pragma once
#include <set>
#include <string>
static const std::set<std::string> RESERVED_NAMES = {
    "_random_", "_mouse_", "_edge_", "_myself_", "_stage_"};
void new_sprite(std::string name);
void delete_sprite(std::string name);