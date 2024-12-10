
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

target_link_libraries(receiver CONAN_PKG::stonex-cms-amqp-lib common Dbghelp)

set_property(TARGET receiver PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "url=failover:(localhost:5672,localhost:5673)?maxReconnectAttempts=3 user=CMSTEST password=CMSTEST destination_type=topic destination=Transaction::Transaction/consumer1 iddle_timeout=0, logger_config=bin/receiver_logger.xml")
set_property(TARGET receiver PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LOGS_ROOT "Receiver1")
configure_file(${PROJECT_SOURCE_DIR}/message_receiver.bat ${CMAKE_CURRENT_BINARY_DIR}/../bin/message_receiver.bat COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/run_test_receiver.bat ${CMAKE_CURRENT_BINARY_DIR}/../bin/run_test_receiver.bat COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/logger.xml.in ${CMAKE_CURRENT_BINARY_DIR}/../bin/receiver_logger.xml)