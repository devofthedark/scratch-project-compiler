#include "StackPop.hpp"

#include <iostream>

void StackPop::print(int depth, std::string prefix) {
    std::cout << depth_prefix(depth, prefix) << "StackPop\n";
}

std::string StackPop::compile(json &work) const {
    std::string pop_id = generate_id();
    work[pop_id] = {
        {"opcode", "data_deleteoflist"},
        {"next", nullptr}, // NOLINTNEXTLINE
        {"inputs", {{"INDEX", json::array({1, json::array({7, "1"})})}}},
        {"fields",
         {{"LIST",
           json::array({"__scratch_compiler_return_stack", "__scratch_compiler_return_stack"})}}},
        {"shadow", false},
        {"topLevel", false}};
    return pop_id;
}