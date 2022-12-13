
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.16)
project(stonex-cms-amqp-lib LANGUAGES CXX)



set(CMAKE_CXX_STANDARD 17)

file(GLOB sourceFiles ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/cms/*.cpp ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/*.cpp ${PROJECT_SOURCE_DIR}/API/*.cpp)
file(GLOB headerFiles ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/cms/*.h ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/*.h ${PROJECT_SOURCE_DIR}/API/*.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sourceFiles} ${headerFiles})

if(BUILD_SHARED_LIBS)
	add_library(stonex-cms-amqp-lib SHARED ${sourceFiles} ${headerFiles})
else()
	add_library(stonex-cms-amqp-lib STATIC ${sourceFiles} ${headerFiles})
endif()


if(CONAN_BUILD)
	target_link_libraries(stonex-cms-amqp-lib PUBLIC CONAN_PKG::red-hat-amq-clients-c++)
	target_include_directories(stonex-cms-amqp-lib BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/ ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/)
else()
	if(BUILD_SHARED_LIBS)
		target_link_libraries(stonex-cms-amqp-lib PUBLIC qpid-proton-core${CMAKE_DEBUG_POSTFIX} qpid-proton-cpp${CMAKE_DEBUG_POSTFIX} qpid-proton-proactor${CMAKE_DEBUG_POSTFIX} qpid-proton${CMAKE_DEBUG_POSTFIX})		
		add_definitions(-DCMS_EXPORTS -DCMS_DLL)
	else()
		add_compile_definitions(PN_CPP_DECLARE_STATIC)
		set_property(TARGET stonex-cms-amqp-lib PROPERTY   MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
		target_link_libraries(stonex-cms-amqp-lib PUBLIC qpid-proton-core-static${CMAKE_DEBUG_POSTFIX} qpid-proton-cpp-static${CMAKE_DEBUG_POSTFIX} qpid-proton-proactor-static${CMAKE_DEBUG_POSTFIX} qpid-proton-static${CMAKE_DEBUG_POSTFIX} jsoncpp)
	endif()
	
	target_include_directories(stonex-cms-amqp-lib BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/activemq-cpp/src/main/ ${PROJECT_SOURCE_DIR}/stonex-cms-amqp-cpp/  ${QPID_INCLUDES}/include ${QPID_INCLUDES}/include/cpp ${QPID_INCLUDES}/include/c  ${BOOST_JSON_INCLUDES})	
	target_link_directories(stonex-cms-amqp-lib PUBLIC  ${QPID_INCLUDES}/lib ${JSONCPP_LIBRARIES})
endif()




SET(COMMAND_STRING "")

foreach(X IN LISTS CONAN_LIBS_RED-HAT-AMQ-CLIENTS-C++)
    LIST(APPEND COMMAND_STRING ${CONAN_LIB_DIRS_RED-HAT-AMQ-CLIENTS-C++}/${X}.lib) 
    message("added lib  "  ${CONAN_LIB_DIRS_RED-HAT-AMQ-CLIENTS-C++}/${X}.lib)
endforeach()

foreach(X IN LISTS CONAN_LIBS_JSONCPP)
    LIST(APPEND COMMAND_STRING ${CONAN_LIB_DIRS_JSONCPP}/${X}.lib) 
    message("added lib  "  ${CONAN_LIB_DIRS_JSONCPP}/${X}.lib)
endforeach()


if(WIN32)
Set(CUSTOM_COMMAND_ARCH "X64")
else()
Set(CUSTOM_COMMAND_ARCH "X86")
endif()

if(CONAN_BUILD)
	add_custom_target(stonex-cms-amqp-full-lib 
			COMMENT "linking into one static library lib.exe /MACHINE:${CUSTOM_COMMAND_ARCH} /OUT:../lib/stonex-cms-amqp-full-lib.lib ${COMMAND_STRING}" 
			DEPENDS stonex-cms-amqp-lib
			
			COMMAND "lib.exe "  "/MACHINE:${CUSTOM_COMMAND_ARCH}" "/OUT:../lib/stonex-cms-amqp-full-lib.lib" ${COMMAND_STRING} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/stonex-cms-amqp-lib.lib
	   )
endif()

message("compilation parameters \n c++ ${CMAKE_CXX_STANDARD} \n shared ${BUILD_SHARED_LIBS} \n build from conan ${CONAN_BUILD} \n debug postfix ${CMAKE_DEBUG_POSTFIX} \n PN_CPP_DECLARE_STATIC ${PN_CPP_DECLARE_STATIC} \n cms export ${CMS_EXPORTS}  \n cms dll  ${CMS_DLL}  \n stx export ${STX_EXPORTS}")