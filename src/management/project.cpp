#include "project.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "config.hpp"
#include "strings.hpp"
template <> struct std::formatter<std::filesystem::path> : std::formatter<std::string> {};
using json = nlohmann::json;
void new_project(const std::string &dir) {
    std::filesystem::path selected_dir(dir);
    std::filesystem::create_directory(selected_dir / "costumes");
    std::filesystem::create_directory(selected_dir / "sounds");
    std::ofstream ofs(selected_dir / "script.spc");
    ofs.close();
    std::ofstream ofs2(selected_dir / "config.json");
    ofs2 << std::setw(4) << json(ProjectConfig()) << '\n';
    ofs2.close();
    std::cout << "YES\n";
}
bool is_project() {
    std::filesystem::path current_dir = std::filesystem::current_path();
    return std::filesystem::exists(current_dir / "costumes")
        && std::filesystem::exists(current_dir / "sounds")
        && std::filesystem::exists(current_dir / "script.spc")
        && std::filesystem::exists(current_dir / "config.json")
        && std::filesystem::is_directory(current_dir / "costumes")
        && std::filesystem::is_directory(current_dir / "sounds");
}