
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp-lib LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/cms/*.cpp ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/*.cpp ${PROJECT_SOURCE_DIR}/API/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/cms/*.h ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/*.h ${PROJECT_SOURCE_DIR}/API/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_library(stonex-cms-amqp-lib STATIC ${sourceFiles} ${headerFiles})
 
target_compile_definitions(stonex-cms-amqp-lib PRIVATE PN_CPP_DECLARE_STATIC)

target_include_directories(stonex-cms-amqp-lib BEFORE PRIVATE  ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/)
target_include_directories(stonex-cms-amqp-lib PUBLIC ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/ ${PROJECT_SOURCE_DIR})

if(CONAN_BUILD)
	target_link_libraries(stonex-cms-amqp-lib PUBLIC CONAN_PKG::red-hat-amq-clients-c++ CONAN_PKG::jsoncpp CONAN_PKG::stonex-logger CONAN_PKG::fmt)
else()
	set_property(TARGET stonex-cms-amqp-lib PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	target_include_directories(stonex-cms-amqp-lib BEFORE PUBLIC  ${QPID_INCLUDES} ${QPID_INCLUDES}/cpp ${QPID_INCLUDES}/c)
	target_link_directories(stonex-cms-amqp-lib PUBLIC ${QPID_LIBRARIES})
	target_link_libraries(stonex-cms-amqp-lib PUBLIC qpid-proton-core-static qpid-proton-cpp-static qpid-proton-proactor-static qpid-proton-static)
endif()

if(WIN32)
Set(CUSTOM_COMMAND_ARCH "X64")
else()
Set(CUSTOM_COMMAND_ARCH "X86")
endif()