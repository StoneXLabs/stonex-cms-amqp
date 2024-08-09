
cmake_minimum_required(VERSION 3.16)
project(examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
    
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(common)
add_subdirectory(tests)
add_subdirectory(receiver)
add_subdirectory(sender)
add_subdirectory(thread-sender)



