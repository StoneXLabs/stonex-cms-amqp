
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_library(common STATIC ${sourceFiles} ${headerFiles})
target_link_libraries(common CONAN_PKG::stonex-cms-amqp-lib)

