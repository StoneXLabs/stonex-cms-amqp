
cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

#option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if(CONAN_BUILD)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup(TARGETS)
else()
	if(NOT QPID_INCLUDES)
		message(FATAL_ERROR "QPID PROTON path missing, QPID_INCLUDES not defined")
	endif()
endif()

if(BUILD_STATIC_LIBS)
	add_compile_definitions(PN_CPP_DECLARE_STATIC)
else()
endif()

if(CMAKE_BUILD_TYPE STREQUAL  "Debug")
	SET(CMAKE_DEBUG_POSTFIX "d")
endif()
    
add_subdirectory(stonex-cms-amqp-lib)

if(BUILD_TEST)
	add_subdirectory(stonex-cms-amqp-unittests)
endif()
