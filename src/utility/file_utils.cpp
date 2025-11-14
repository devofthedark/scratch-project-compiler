#include "file_utils.hpp"

namespace file_utils {
std::ifstream open_file(const std::string &path, std::ios::openmode mode) {
    std::ifstream file(path, mode);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return file;
}
std::ifstream open_file(const std::filesystem::path &path, std::ios::openmode mode) {
    return open_file(path.string(), mode);
}
std::ofstream write_file(const std::string &path) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }
    return file;
}
std::ofstream write_file(const std::filesystem::path &path) {
    return write_file(path.string());
}
} // namespace file_utils