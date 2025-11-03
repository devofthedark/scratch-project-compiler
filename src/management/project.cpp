#include "project.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "config.hpp"
#include "strings.hpp"
template <> struct std::formatter<std::filesystem::path> : std::formatter<std::string> {};
using json = nlohmann::json;
namespace {
void can_create_check(const std::filesystem::path &dir) {
    // check if we have read/write permissions, and the directory is empty
    if (!std::filesystem::exists(dir)) {
        std::cerr << std::format(strings::errors::PATH_NOT_EXIST, dir) << '\n';
        std::exit(1);
    }
    if (!std::filesystem::is_directory(dir)) {
        std::cerr << std::format(strings::errors::PATH_NOT_DIRECTORY, dir) << '\n';
        std::exit(1);
    }
    if (!std::filesystem::is_empty(dir)) {
        std::cerr << std::format(strings::errors::PATH_NOT_EMPTY, dir) << '\n';
        std::exit(1);
    }
    // test writing a temporary file
    std::filesystem::path test_file = dir / "test";
    std::ofstream ofs(test_file);
    if (!ofs) {
        std::cerr << std::format(strings::errors::CANNOT_WRITE, dir) << '\n';
        std::exit(1);
    }
    ofs.close();
    std::filesystem::remove(test_file);
}
} // namespace
void new_project(const std::string &dir) {
    std::filesystem::path selected_dir(dir);
    can_create_check(selected_dir);
    std::filesystem::create_directory(selected_dir / "costumes");
    std::filesystem::create_directory(selected_dir / "sounds");
    std::ofstream ofs(selected_dir / "script.scratch");
    ofs.close();
    std::ofstream ofs2(selected_dir / "config.json");
    ofs2 << std::setw(4) << json(ProjectConfig()) << '\n';
    ofs2.close();
    std::cout << "YES\n";
}
bool is_project() {
    std::filesystem::path current_dir = std::filesystem::current_path();
    if (!std::filesystem::exists(current_dir / "script.scratch")) {
        return false;
    }
    if (!std::filesystem::exists(current_dir / "config.json")) {
        return false;
    }
    if (!std::filesystem::exists(current_dir / "costumes")) {
        return false;
    }
    if (!std::filesystem::exists(current_dir / "sounds")) {
        return false;
    }
    return true;
}