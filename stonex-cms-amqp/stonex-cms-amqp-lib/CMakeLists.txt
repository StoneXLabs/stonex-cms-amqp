
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp-lib LANGUAGES CXX)

find_package(red-hat-amq-client)
find_package(log4cxx)

file(GLOB ActiveMQAPI ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/cms/*)

file(GLOB sourceFiles 
	${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/cms/*.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/ProtonCppLibrary.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/ConnectionFactory.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Connection.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/ConnectionMetadata.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Session.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/MessageProducer.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/MessageConsumer.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Queue.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/TemporaryQueue.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Topic.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/TemporaryTopic.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Message.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/TextMessage.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/BytesMessage.cpp
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Protocol/utils.cpp
)
file(GLOB headerFiles
	${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/cms/*.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/ProtonCppLibrary.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/ConnectionFactory.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Connection.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/ConnectionMetadata.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Session.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/MessageProducer.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/MessageConsumer.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Queue.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/TemporaryQueue.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Topic.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/TemporaryTopic.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Message.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/TextMessage.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/BytesMessage.h
	${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/Protocol/utils.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_library(stonex-cms-amqp-lib STATIC ${sourceFiles} ${headerFiles})
 
target_compile_definitions(stonex-cms-amqp-lib PRIVATE PN_CPP_DECLARE_STATIC)

target_include_directories(stonex-cms-amqp-lib PUBLIC ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/ ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/ ${PROJECT_SOURCE_DIR})

target_link_libraries(stonex-cms-amqp-lib PUBLIC red-hat-amq-client::red-hat-amq-client log4cxx::log4cxx Dbghelp)

if(WIN32)
Set(CUSTOM_COMMAND_ARCH "X64")
else()
Set(CUSTOM_COMMAND_ARCH "X86")
endif()