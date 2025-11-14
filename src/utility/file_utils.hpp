#include <filesystem>
#include <fstream>

namespace file_utils {
std::ifstream open_file(const std::string &path, std::ios::openmode mode = std::ios::binary);
std::ifstream open_file(const std::filesystem::path &path,
                        std::ios::openmode mode = std::ios::binary);
std::ofstream write_file(const std::string &path);
std::ofstream write_file(const std::filesystem::path &path);
} // namespace file_utils
