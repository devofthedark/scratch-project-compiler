#include "StackTop.hpp"

#include <iostream>

void StackTop::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "StackTop\n";
}

std::string StackTop::compile(json &work) const {
    std::string top_id = generate_id();
    work[top_id] = {
        {"opcode", "data_itemoflist"},
        {"next", nullptr}, // NOLINTNEXTLINE
        {"inputs", {{"INDEX", json::array({1, json::array({7, "1"})})}}},
        {"fields",
         {{"LIST",
           json::array({"__scratch_compiler_return_stack", "__scratch_compiler_return_stack"})}}},
        {"shadow", false},
        {"topLevel", false}};
    return top_id;
}