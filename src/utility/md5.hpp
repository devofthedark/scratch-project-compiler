#pragma once
#include <cstdint>
#include <string> /// Used for strings

namespace hashing::md5 {
std::string sig2hex(void *sig);
void *hash_bs(const void *input_bs, uint64_t input_size);
} // namespace hashing::md5