#include "StdcallHook.hpp"

#include <memory>

#include "compiler/Expression.hpp"

namespace {
// valid field names are given as FIELD_BEGIN, aval, bval, ..., FIELD_END, ("inputs" or "fields")
// , (opcode if in "inputs"), key in the json, given value
inline std::set<std::string> valid_field_names(const std::vector<std::unique_ptr<Expression>> &args,
                                               size_t &pos) {
    std::set<std::string> ret_val;
    while (pos < args.size()) {
        auto given_str = args[pos++]->str_if_string_literal();
        if (given_str == "FIELD_END") {
            return ret_val;
        }
        ret_val.insert(given_str);
    }
    throw std::runtime_error("Internal Stdcall exception: fields parsing has no end");
}
} // namespace
std::string StdcallHook::compile(json &work) const {
    std::string opcode = args[0]->str_if_string_literal();
    std::string stdcall_id = generate_id();
    work[stdcall_id] = json::object({{"opcode", opcode},
                                     {"inputs", json::object({})},
                                     {"fields", json::object({})},
                                     {"shadow", false},
                                     {"topLevel", false}});
    size_t pos = 1;
    size_t passed_args_pos = 0;
    for (; pos < args.size(); pos += 2) {
        auto given_str = args[pos]->str_if_string_literal();
        if (given_str == "FIELD_BEGIN") {
            auto valid_names = valid_field_names(args, ++pos);
            auto area = args[pos++]->str_if_string_literal();
            std::string opcode;
            if (area == "inputs") {
                opcode = args[pos++]->str_if_string_literal();
            }
            auto json_key = args[pos]->str_if_string_literal();
            auto passed_val = num_value(passed_args[passed_args_pos++])[1][1];
            if (!valid_names.contains(passed_val)) {
                throw std::runtime_error("StdcallHook field parsing error");
            }
            if (area == "inputs") {
                auto tmp = generate_id();
                work[tmp] = json::object(
                    {{"opcode", opcode},
                     {"inputs", json::object({})},
                     {"fields", json::object({{json_key, json::array({passed_val, nullptr})}})},
                     {"shadow", true},
                     {"topLevel", false}});
                work[stdcall_id]["inputs"][json_key] = json::array({1, tmp});
                continue;
            }

            work[stdcall_id]["fields"][json_key] = json::array({passed_val, nullptr});
            continue;
        }
        work[stdcall_id]["inputs"][given_str] = num_value(passed_args[passed_args_pos++]);
    }
    return stdcall_id;
}