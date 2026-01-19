#include "StdcallHook.hpp"

#include <format>
#include <iostream>
#include <memory>

#include "compiler/Expression.hpp"
#include "exceptions/LanguageErrors.hpp"

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

inline void throw_error(const std::set<std::string> &valid_names,
                        const std::string &passed_val,
                        const std::string &opcode) {
    std::string err_str = std::format(
        R"(Hook to scratch block with opcode "{}" failed: a field was given the invalid value of "{}". Valid values are )",
        opcode,
        passed_val);
    for (const auto &name : valid_names) {
        err_str += std::format("\"{}\", ", name);
    }
    throw TypeError(err_str.substr(0, err_str.length() - 2));
}
} // namespace
std::string StdcallHook::compile(json &work) const {
    std::string opcode;
    try {
        opcode = args[0]->str_if_string_literal();
    } catch (TypeError &e) {
        std::cerr << e.what() << '\n';
        throw TypeError("In Stdcall Hook");
    }
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
            std::set<std::string> valid_names;
            std::string internal_opcode;
            std::string area;
            std::string json_key;
            std::string passed_val;
            try {
                valid_names = valid_field_names(args, ++pos);
                area = args[pos++]->str_if_string_literal();
                if (area == "inputs") {
                    internal_opcode = args[pos++]->str_if_string_literal();
                }
                json_key = args[pos]->str_if_string_literal();
            } catch (TypeError &e) {
                throw TypeError(std::format("In hook to scratch block with opcode \"{}\"", opcode));
            }
            passed_val = num_value(passed_args[passed_args_pos++])[1][1];
            if (!valid_names.contains(passed_val)) {
                throw_error(valid_names, passed_val, opcode);
            }
            if (area == "inputs") {
                auto tmp = generate_id();
                work[tmp] = json::object(
                    {{"opcode", internal_opcode},
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