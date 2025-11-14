cmake_minimum_required(VERSION 3.14)
project(spc VERSION "0.0.1"
             LANGUAGES CXX
             DESCRIPTION "The Scratch Project Compiler")
set(VERSION_SUFFEX "-dev1")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CXXOPT "-O0 -g")
set(CXXWARN "-Wall -Wextra -Werror")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXOPT} ${CXXWARN}")

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
configure_file("${SRC_DIR}/version.h.in" "${SRC_DIR}/version.h" @ONLY)
include_directories(${PROJECT_BINARY_DIR}/src)
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG 3eda91b2e1ce7d569f84ba295507c4cd8fd96910 # v3.2.0
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(json 
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG 55f93686c01528224f448c19128836e7df245f72 # v3.12.0
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum
    GIT_TAG e046b69a3736d314fad813e159b1c192eaef92cd # v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

set(SCOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/compile.cpp"
    "${SRC_DIR}/management/project.cpp"
    "${SRC_DIR}/management/sprites.cpp"
    "${SRC_DIR}/compiler/ASTNode.cpp"
    "${SRC_DIR}/compiler/BinaryOp.cpp"
    "${SRC_DIR}/compiler/BlockStatement.cpp"
    "${SRC_DIR}/compiler/Expression.cpp"
    "${SRC_DIR}/compiler/FunctionArgument.cpp"
    "${SRC_DIR}/compiler/FunctionBody.cpp"
    "${SRC_DIR}/compiler/FunctionDeclaration.cpp"
    "${SRC_DIR}/compiler/FunctionExpression.cpp"
    "${SRC_DIR}/compiler/IfStatement.cpp"
    "${SRC_DIR}/compiler/Lexer.cpp"
    "${SRC_DIR}/compiler/LiteralNodes.cpp"
    "${SRC_DIR}/compiler/VariableAssignment.cpp"
    "${SRC_DIR}/compiler/VariableDeclaration.cpp"
    "${SRC_DIR}/compiler/VariableExpression.cpp"
    "${SRC_DIR}/compiler/WhileLoopStatement.cpp"
    "${SRC_DIR}/compiler/NotOperator.cpp"
    "${SRC_DIR}/compiler/Parser.cpp"
    "${SRC_DIR}/compiler/ReturnStatement.cpp"
    "${SRC_DIR}/compiler/Statement.cpp"
    "${SRC_DIR}/compiler/VariableAssignment.cpp"
    "${SRC_DIR}/compiler/VariableDeclaration.cpp"
    "${SRC_DIR}/compiler/VariableExpression.cpp"
    "${SRC_DIR}/compiler/WhileLoopStatement.cpp"
    "${SRC_DIR}/exceptions/LanguageErrors.cpp"
    "${SRC_DIR}/utility/md5.cpp"
    "${SRC_DIR}/utility/file_utils.cpp"
    )

add_executable(spc ${SCOURCES})
target_include_directories(spc PRIVATE "${SRC_DIR}")
target_link_libraries(spc PRIVATE argparse
    nlohmann_json::nlohmann_json magic_enum)
    
