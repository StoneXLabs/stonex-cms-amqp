
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp-test LANGUAGES CXX)


#set(CMAKE_CXX_STANDARD 17)


file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/src/*.cpp  ${PROJECT_SOURCE_DIR}/schema/*.cc)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/include/*.h  ${PROJECT_SOURCE_DIR}/schema/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(stonex-cms-amqp-test ${sourceFiles} ${headerFiles})
 
target_compile_definitions(stonex-cms-amqp-test PRIVATE PN_CPP_DECLARE_STATIC)
target_include_directories(stonex-cms-amqp-test BEFORE PRIVATE  ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/schema/)

target_link_libraries(stonex-cms-amqp-test PUBLIC stonex-cms-amqp-lib Secur32 Crypt32 Ws2_32 CONAN_PKG::protobuf)