cmake_minimum_required(VERSION 3.14)
project(cilc VERSION "0.0.1"
             LANGUAGES CXX
             DESCRIPTION "The Scratch Compiler")
set(VERSION_SUFFEX "-dev1")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CXXOPT "-O0 -g")
set(CXXWARN "-Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wconversion -Wshift-overflow -Wcast-qual -Wcast-align")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXOPT} ${CXXWARN}")

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
configure_file("${SRC_DIR}/version.h.in" "${SRC_DIR}/version.h" @ONLY)
include_directories(${PROJECT_BINARY_DIR}/src)
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        b43b2f9537db6a1ee66c4ad74b8497853c928d5f)
FetchContent_MakeAvailable(fmt)
FetchContent_Declare(json 
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG 697c7e557c84c37e88beca4eb591e4ac7b0598b8)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum
    GIT_TAG a413fcc9c46a020a746907136a384c227f3cd095
)
FetchContent_MakeAvailable(magic_enum)
set(SCOURCES
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/compile.cpp"
    "${SRC_DIR}/management/project.cpp"
    "${SRC_DIR}/management/sprites.cpp"
    "${SRC_DIR}/compiler/Lexer.cpp"
    "${SRC_DIR}/compiler/ASTNode.cpp"
    "${SRC_DIR}/compiler/BinaryOp.cpp"
    "${SRC_DIR}/compiler/BlockStatement.cpp"
    "${SRC_DIR}/compiler/FunctionDeclaration.cpp"
    "${SRC_DIR}/compiler/FunctionExpression.cpp"
    "${SRC_DIR}/compiler/IfStatement.cpp"
    "${SRC_DIR}/compiler/LiteralNodes.cpp"
    "${SRC_DIR}/compiler/VariableAssignment.cpp"
    "${SRC_DIR}/compiler/VariableDeclaration.cpp"
    "${SRC_DIR}/compiler/VariableExpression.cpp"
    "${SRC_DIR}/compiler/WhileLoopStatement.cpp"
    "${SRC_DIR}/compiler/Parser.cpp"
    "${SRC_DIR}/compiler/VariableAssignment.cpp"
    "${SRC_DIR}/compiler/VariableDeclaration.cpp"
    "${SRC_DIR}/compiler/VariableExpression.cpp"
    "${SRC_DIR}/compiler/WhileLoopStatement.cpp"
    "${SRC_DIR}/compiler/ReturnStatement.cpp"
    )

add_executable(scratchc ${SCOURCES})
target_include_directories(scratchc PRIVATE "${SRC_DIR}")
target_link_libraries(scratchc PRIVATE argparse
    fmt::fmt nlohmann_json::nlohmann_json magic_enum)
    
