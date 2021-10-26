cmake_minimum_required(VERSION 3.15)
project(katsu)

find_package(Clang REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(thirdparty/fmtlib)

add_executable(katsu src/main.cpp include/ast_visitor.h include/types.h src/ast_visitor.cpp src/data_writer.cpp)

target_link_libraries(katsu LLVMSupport libclang fmt)
target_include_directories(katsu PRIVATE include ${LLVM_INCLUDE_DIR})
