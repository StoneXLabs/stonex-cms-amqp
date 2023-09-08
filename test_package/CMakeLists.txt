cmake_minimum_required(VERSION 3.1)
project(PackageTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)


file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/include/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(stonex-cms-amqp-tests  
	${sourceFiles} 
	${headerFiles}
	)

target_include_directories(stonex-cms-amqp-tests
	PUBLIC ${PROJECT_SOURCE_DIR}/include
	)

target_link_libraries(stonex-cms-amqp-tests ${CONAN_LIBS}
	secur32 
	crypt32 
	Ws2_32
	)

configure_file(${PROJECT_SOURCE_DIR}/config/core_test.config ${CMAKE_CURRENT_BINARY_DIR}/bin/core_test.config COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/config/core_messages_test.config ${CMAKE_CURRENT_BINARY_DIR}/bin/core_messages_test.config COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/config/logger.properties ${CMAKE_CURRENT_BINARY_DIR}/bin/logger.properties COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/config/logger.xml ${CMAKE_CURRENT_BINARY_DIR}/bin/logger.xml COPYONLY)


# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
