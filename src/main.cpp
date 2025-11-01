#include <fmt/ostream.h>

#include <argparse/argparse.hpp>
#include <magic_enum/magic_enum.hpp>

#include "compile.hpp"
#include "compiler/Lexer.hpp"
#include "compiler/Parser.hpp"
#include "management/project.hpp"
#include "management/sprites.hpp"
#include "strings.hpp"
#include "version.h"

namespace {
inline void printversion() {
    fmt::println(strings::generic::version_string,
                 VERSION,
                 __DATE__,
                 __TIME__,
                 COMPILER,
                 COMPILER_VERSION,
                 OS,
                 ARCH);
    std::exit(0);
}
} // namespace
template <> struct fmt::formatter<argparse::ArgumentParser> : ostream_formatter {};
int main(int argc, char *argv[]) {
    std::string dirname;
    std::string sprite_name;

    argparse::ArgumentParser program("sratchc", VERSION, argparse::default_arguments::help);
    program.add_description(DESCRIPTION);

    program.add_argument("-v", "--version")
        .help("Show version information and exits")
        .flag()
        .action([&](const auto &) { printversion(); });

    argparse::ArgumentParser new_command("new", VERSION, argparse::default_arguments::help);
    new_command.add_description("Creates a new project");
    new_command.add_argument("directory")
        .help("The directory to create the project in")
        .default_value(std::string("."))
        .store_into(dirname);
    program.add_subparser(new_command);

    argparse::ArgumentParser sprite_command("sprite", VERSION, argparse::default_arguments::help);
    sprite_command.add_description("Manage project sprites");

    argparse::ArgumentParser sprite_new_command("new", VERSION, argparse::default_arguments::help);
    sprite_new_command.add_description("Creates a new sprite");
    sprite_new_command.add_argument("name").help("The name of the sprite").store_into(sprite_name);
    sprite_command.add_subparser(sprite_new_command);

    argparse::ArgumentParser sprite_delete_command("delete",
                                                   VERSION,
                                                   argparse::default_arguments::help);
    sprite_delete_command.add_description("Deletes a sprite");
    sprite_delete_command.add_argument("name")
        .help("The name of the sprite")
        .store_into(sprite_name);
    sprite_command.add_subparser(sprite_delete_command);

    program.add_subparser(sprite_command);
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        fmt::println(stderr, "{}\n{}", err.what(), program);
        std::exit(1);
    }
    if (program.is_subcommand_used(new_command)) {
        // create a new project
        std::cout << "new command used\n";
        std::cout << dirname << '\n';
        new_project(dirname);
    } else if (program.is_subcommand_used(sprite_command)) {
        fmt::println("Sprite command used");
        if (!is_project()) {
            fmt::println(stderr, "Error: not in a project directory");
            std::exit(1);
        }
        if (sprite_command.is_subcommand_used(sprite_new_command)) {
            fmt::println("New sprite command used");
            fmt::println("Sprite name: {}", sprite_name);
            new_sprite(sprite_name);

        } else if (sprite_command.is_subcommand_used(sprite_delete_command)) {
            fmt::println("Delete sprite command used");
            fmt::println("Sprite name: {}", sprite_name);
            delete_sprite(sprite_name);
        }
    } else {
        fmt::println(stderr, "Error: no command given\n{}", program);
        // std::exit(1);
    }
    std::vector<Token> tokens = tokenize("test.txt");
    // fmt::println("Tokens:");
    // for (const auto &token : tokens) {
    //     fmt::println("{}: {} (line {})", magic_enum::enum_name(token.type),
    //                  token.value, token.line);
    // }
    BlockStatement ast = parse_tokens(tokens.begin(), tokens.end());
    ast.print();
    TypeCheckerContext ctx;
    // auto check = AST.typeCheck(ctx);
    // fmt::println("AST Type Check Result: {}", magic_enum::enum_name(check));
    auto tmp = compile_project(ctx, ast);
    fmt::println("Compiled JSON:\n{}", tmp.dump(4));
}