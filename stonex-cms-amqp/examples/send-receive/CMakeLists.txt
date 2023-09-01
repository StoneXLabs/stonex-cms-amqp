
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(send-receive LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(send-receive ${sourceFiles} ${headerFiles})




if(WIN32)
  target_link_libraries(send-receive Secur32 Ws2_32 Crypt32)
endif()

if(CONAN_BUILD)
	target_link_libraries(send-receive stonex-cms-amqp-lib)
else()
	set_property(TARGET send-receive PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	include(FetchContent)		
	add_dependencies(send-receive stonex-cms-amqp-lib)
	target_link_directories(send-receive PUBLIC  ../lib/Debug ${QPID_INCLUDES}/lib ${JSONCPP_LIBRARIES} ${GTEST_LIBRARIES} )
	target_include_directories(send-receive BEFORE PRIVATE ../stonex-cms-amqp-lib/API ../stonex-cms-amqp-lib/stonex-cms-amqp-cpp ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/ ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/   ${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/teamcity-cpp/gtest ${GTEST_INCLUDES})
	target_link_libraries(send-receive stonex-cms-amqp-lib gtest_main gmock_main jsoncpp)
endif()

configure_file(${PROJECT_SOURCE_DIR}/logger.xml ${CMAKE_CURRENT_BINARY_DIR}/logger.xml COPYONLY)    