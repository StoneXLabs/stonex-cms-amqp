
cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD  20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if(BUILD_SHARED_LIBS)
else()
	add_compile_definitions(PN_CPP_DECLARE_STATIC)
endif()

if(CMAKE_BUILD_TYPE STREQUAL  "Debug")
	SET(CMAKE_DEBUG_POSTFIX "d")
endif()
    
add_subdirectory(stonex-cms-amqp-lib)

add_subdirectory(UnitTest)
add_subdirectory(Test)

