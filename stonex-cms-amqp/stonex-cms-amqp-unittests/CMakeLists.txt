
cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp-unittests)

set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/*.cpp   ${PROJECT_SOURCE_DIR}/teamcity-cpp/gtest/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/*.h ${PROJECT_SOURCE_DIR}/teamcity-cpp/gtest/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

add_executable(stonex-cms-amqp-unittests ${sourceFiles} ${headerFiles})



if(WIN32)
  target_link_libraries(stonex-cms-amqp-unittests Secur32 Ws2_32 Crypt32)
endif()

if(CONAN_BUILD)
	add_dependencies(stonex-cms-amqp-unittests stonex-cms-amqp-full-lib)
	target_link_libraries(stonex-cms-amqp-unittests ../lib/stonex-cms-amqp-full-lib CONAN_PKG::gtest )
else()
	include(FetchContent)
	FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	
	add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY)
		
#	set_property(TARGET stonex-cms-amqp-unittests PROPERTY   MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	add_dependencies(stonex-cms-amqp-unittests stonex-cms-amqp-lib)
	target_link_directories(stonex-cms-amqp-unittests PUBLIC  ../stonex-cms-amqp-lib/Debug ${QPID_INCLUDES}/lib ${JSONCPP_LIBRARIES} ${GTEST_LIBRARIES} )
	target_include_directories(stonex-cms-amqp-unittests BEFORE PRIVATE ../stonex-cms-amqp-lib/Debug ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/ ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/  ${QPID_INCLUDES}/include ${QPID_INCLUDES}/include/cpp ${QPID_INCLUDES}/include/c ${GTEST_INCLUDE_DIRS})
	target_link_libraries(stonex-cms-amqp-unittests stonex-cms-amqp-lib gtest_main gmock_main)
endif()



target_include_directories(stonex-cms-amqp-unittests BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/../stonex-cms-amqp-lib/API ${PROJECT_SOURCE_DIR}/../stonex-cms-amqp-lib/stonex-cms-amqp-cpp  ${PROJECT_SOURCE_DIR}/../stonex-cms-amqp-lib/activemq-cpp/src/main ${CONAN_INCLUDE_DIRS_RED-HAT-AMQ-CLIENTS-C++} ${PROJECT_SOURCE_DIR}/teamcity-cpp/gtest/)
