# stonex-cms-amqp
New generation C++ (CMS) client library on top of Qpid Proton AMQP 

==========================================================================

[stonex-cms-amqp] is wrapper library for Qpid Proton (https://qpid.apache.org/proton)
It is aimed to provide amqp client functionality [Qpid Proton] compatible with CMS interface [ApacheActiveMq]

Requirements
	
AMQ Clients 2.10.4 (https://access.redhat.com/jbossnetwork/restricted/softwareDetail.html?softwareId=104014&product=jboss.amq.clients&version=2.10.4&downloadType=distributions)

Repository structure
==========================================================================
messaging-cms-client	
	stonex-cms-amqp						//library project with unittests
		stonex-cms-amqp-lib
		stonex-cms-amqp-unittests
	stonex-cms-amqp-test-framework		//test framework wrapping stonex-cms-amqp-lib to perform integration tests
	stonex-cms-amqp-tests				//test cases implementation

Build instructions

	Requirements
	
		stonex-cms-amqp
			QPID PROTON CLIENT
				amq-clients-2.10.4
				jsoncpp 1.9.5
				openssl 3.2.0
				
		stonex-cms-amqp-test-framework
			stonex-cms-amqp-lib
			boost 1.78.0
				
		stonex-cms-amqp-tests
			stonex-cms-amqp-test-framework
			boost 1.78.0
			
		stonex-cms-amqp-unittests
			stonex-cms-amqp-lib
			gtest 1.10.0
			
		supported compilers
			Visual Studio 16 2019
			Visual Studio 15 2017
			
		
	stonex-cms-amqp	
	==========================================================================
		cmake variables
			BUILD_TEST=<ON|OFF> -if on test project will be built, 
								test project requires properly set variables
								BOOST_ROOT
								GTEST_INCLUDES
								GTEST_LIBRARIES
								else those dependencies are not required
			
		mkdir <build dir>
		
		cd <build dir>	
		
		cmake   ..\stonex-cms-amqp\ -G <compiler>  -DQPID_INCLUDES=<qpid include directory> -DQPID_LIBRARIES=<qpid libraries directory>  -DBUILD_TEST=ON -DJSONCPP_LIBRARIES=<json cpp library directory>  -DBOOST_ROOT=<boost json include directory> -DGTEST_INCLUDES=<google test include directory> -DGTEST_LIBRARIES=<google test libraries>
		
		cmake --build . --config=[Release|Debug]
		
		
		Dependencies
		
		BOOST
		
		download boost 1.78 sources
		
		create build directory in boost sources parent directory
		
			mkdir boost_1_78_0_build
		
		build boost 
		
			cd boost_1_78_0
			
			.\b2.exe -j16 install --prefix=..\boost_1_78_0_build\debug  toolset=msvc-142 variant=release link=shared threading=multi runtime-link=shared address-model=64   --libdir=..\boost_1_78_0_build\16\x86_64\Release_shared\lib --includedir=..\boost_1_78_0_build\16\x86_64\Debug\include
		
		GOOGLE TEST	
			build from cmake
			repository https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
		
		
	stonex-cms-amqp	conan
	==========================================================================
	
	cd messaging-cms-client
	
	conan install . -s os=Windows -s os_build=Windows -s arch=[ x86|x86_64 ] -s arch_build=[ x86|x86_64 ] -s compiler="Visual Studio" -s compiler.version=[ 15|16 ] -s compiler.runtime=[ MT|MTd ] -s build_type=[ Release|Debug]  -if <optional conan generated files install folder>
	
	conan build . -if <the same like in install step if used> -bf <optional build folder>
	
	! optional create local package conan export-pkg <user>/<channel>
	
	
	stonex-cms-amqp-test-framework conan
	==========================================================================
	
	!build stonex-cms-amqp and export package to local cache
	
	cd messaging-cms-client/stonex-cms-amqp-test-framework
	
	conan install . -s os=Windows -s os_build=Windows -s arch=[ x86|x86_64 ] -s arch_build=[ x86|x86_64 ] -s compiler="Visual Studio" -s compiler.version=[ 15|16 ] -s compiler.runtime=[ MT|MTd ] -s build_type=[ Release|Debug]  -if <optional conan generated files install folder>
	
	conan build . -if <the same like in install step if used> -bf <optional build folder>
	
	
	! optional create local package conan export-pkg <user>/<channel>
	
	
	stonex-cms-amqp-tests conan
	==========================================================================
	
	cd messaging-cms-client/stonex-cms-amqp-tests
	
	conan install . -s os=Windows -s os_build=Windows -s arch=[ x86|x86_64 ] -s arch_build=[ x86|x86_64 ] -s compiler="Visual Studio" -s compiler.version=[ 15|16 ] -s compiler.runtime=[ MT|MTd ] -s build_type=[ Release|Debug]  -if <optional conan generated files install folder>
	
	conan build . -if <the same like in install step if used> -bf <optional build folder>
	
	