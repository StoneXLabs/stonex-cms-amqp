
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(tests ${sourceFiles} ${headerFiles})

if(WIN32)
  target_link_libraries(tests Secur32 Ws2_32 Crypt32)
endif()

target_link_libraries(tests CONAN_PKG::stonex-cms-amqp-lib common Dbghelp)

set_property(TARGET tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LOGS_ROOT "Test")
configure_file(${PROJECT_SOURCE_DIR}/logger.xml.in ${CMAKE_CURRENT_BINARY_DIR}/../bin/test_logger.xml)