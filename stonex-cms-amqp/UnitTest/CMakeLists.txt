
cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp-unittests)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/*.h)


find_package(GTest REQUIRED)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(stonex-cms-amqp-unittests ${sourceFiles} ${headerFiles})

if(WIN32)
  target_link_libraries(stonex-cms-amqp-unittests Secur32 Ws2_32 Crypt32)
endif()

add_dependencies(stonex-cms-amqp-unittests stonex-cms-amqp-lib)
target_link_libraries(stonex-cms-amqp-unittests stonex-cms-amqp-lib GTest::GTest)
target_include_directories(stonex-cms-amqp-unittests BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/../stonex-cms-amqp-lib/stonex-cms-amqp-cpp ${CONAN_INCLUDE_DIRS_RED-HAT-AMQ-CLIENT})


