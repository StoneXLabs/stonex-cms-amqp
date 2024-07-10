
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(receiver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(receiver ${sourceFiles} ${headerFiles})

if(WIN32)
  target_link_libraries(receiver Secur32 Ws2_32 Crypt32)
endif()

if(CONAN_BUILD)
	target_link_libraries(receiver stonex-cms-amqp-lib CONAN_PKG::stonex-logger-wrapper)
else()
	set_property(TARGET receiver PROPERTY  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	include(FetchContent)		
	add_dependencies(receiver stonex-cms-amqp-lib CONAN_PKG::fmt)
	target_link_directories(receiver PUBLIC  ../lib/Debug ${QPID_INCLUDES}/lib ${JSONCPP_LIBRARIES} ${GTEST_LIBRARIES} )
	target_include_directories(receiver BEFORE PRIVATE ../stonex-cms-amqp-lib/API ../stonex-cms-amqp-lib/stonex-cms-amqp-cpp ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/ ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/   ${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/teamcity-cpp/gtest ${GTEST_INCLUDES})
	target_link_libraries(receiver stonex-cms-amqp-lib gtest_main gmock_main jsoncpp)
endif()
message(${CMAKE_CURRENT_BINARY_DIR}/../../bin/receiver_logger.xml)
set(LOGS_ROOT "Consumer1")
configure_file(${PROJECT_SOURCE_DIR}/logger.xml.in ${CMAKE_CURRENT_BINARY_DIR}/../../bin/receiver_logger.xml)
#configure_file(${PROJECT_SOURCE_DIR}/logger.xml ${CMAKE_CURRENT_BINARY_DIR}/../../bin/test_logger.xml COPYONLY)    
set_property(TARGET receiver PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "url=failover:(localhost:5672,localhost:5673) user=CMSTEST password=CMSTEST destination_type=topic destination=Transaction::Transaction/consumer1 iddle_timeout=0, logger_config=bin/receiver_logger.xml")
set_property(TARGET receiver PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})