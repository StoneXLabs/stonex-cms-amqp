
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(qpid-test LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)


file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/include/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(qpid-test ${sourceFiles} ${headerFiles})
 
target_compile_definitions(qpid-test PRIVATE PN_CPP_DECLARE_STATIC)
target_include_directories(qpid-test BEFORE PRIVATE  ${PROJECT_SOURCE_DIR}/include/)

target_link_libraries(qpid-test PUBLIC stonex-cms-amqp-lib CONAN_PKG::spdlog CONAN_PKG::spdlog_setup Secur32 Crypt32 Ws2_32 Dbghelp)