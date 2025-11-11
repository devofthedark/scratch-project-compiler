#include "FunctionArgument.hpp"

FunctionArgument::FunctionArgument(std::string name) : VariableExpression(std::move(name)) {}

std::string FunctionArgument::compile(json &work) const {
    std::string report_id = generate_id();
    work[report_id] = {{"opcode", "argument_reporter_string_number"},
                       {"next", nullptr},
                       {"inputs", json::object()},
                       {"fields", {{"VALUE", json::array({getName(), nullptr})}}},
                       {"shadow", false},
                       {"topLevel", false}};
    return report_id;
}