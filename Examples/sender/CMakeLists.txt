
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(sender LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(sender ${sourceFiles} ${headerFiles})

if(WIN32)
  target_link_libraries(sender Secur32 Ws2_32 Crypt32)
endif()

target_link_libraries(sender CONAN_PKG::stonex-cms-amqp-lib common)

set_property(TARGET sender PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "url=failover:(localhost:5672,localhost:5673) user=CMSTEST password=CMSTEST destination_type=topic destination=Transaction data_file=./trade_statement_messages.log  logger_config=bin/sender_logger.xml message_count=180 message_delay=700")
set_property(TARGET sender PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LOGS_ROOT "Producer1")
configure_file(${PROJECT_SOURCE_DIR}/message_sender.bat ${CMAKE_CURRENT_BINARY_DIR}/../bin/message_sender.bat COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/run_test_sender.bat ${CMAKE_CURRENT_BINARY_DIR}/../bin/run_test_sender.bat COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/logger.xml.in ${CMAKE_CURRENT_BINARY_DIR}/../bin/sender_logger.xml)