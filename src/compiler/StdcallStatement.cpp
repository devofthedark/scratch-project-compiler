#include "StdcallStatement.hpp"

std::string StdcallStatement::compile(json &work) const {
    std::string opcode = args[0]->str_if_string_literal();
    std::string stdcall_id = generate_id();
    work[stdcall_id] = json::object({{"opcode", opcode},
                                     {"inputs", json::object({})},
                                     {"fields", json::object({})},
                                     {"shadow", false},
                                     {"topLevel", false}});
    if (args.size() % 2 == 0) {
        throw std::runtime_error("StdcallStatement requires an odd number of arguments");
    }
    for (size_t i = 1; i < args.size(); i += 2) {
        work[stdcall_id]["inputs"][args[i]->str_if_string_literal()] =
            num_value(passed_args[i / 2]);
    }
    return stdcall_id;
}