from conans import ConanFile, CMake, tools
import os

def get_verion_tag():
    git = tools.Git("stonex-cms-amqp")
    try:
        return "%s" % (git.get_tag())
    except Exception as e:
        return e

class StonexCMSAMQPLib(ConanFile):
    name = "stonex-cms-amqp-lib"
    version = "2.0.0"
    license = "Apache 2.0"
    author = "Krzysztof Obrebski krzysztof.obrebski@stonex.com"
    url = "https://github.com/StoneXLabs/stonex-cms-amqp.git"
    description = "amqp cms messaging library"
    topics = ("messaging", "c++", "cpp","proton","amqp","cms")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}  
    generators = ["cmake", "cmake_find_package"]
    
    def export_sources(self):
        self.copy("*", dst="stonex-cms-amqp/", src="stonex-cms-amqp/")

    def requirements(self):
        self.requires("red-hat-amq-client/2.11.0")
        self.requires("log4cxx/1.2.0")  

    def build_requirements(self):
        self.build_requires("gtest/1.10.0")
        self.build_requires("protobuf/3.21.12")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        pass
        
    def build(self):
        protoc_exec = os.path.join(self.deps_cpp_info["protobuf"].bin_paths[0], "protoc")
        for root, _, files in os.walk(os.path.join(self.source_folder,"stonex-cms-amqp","Test","schema")):
            for file in files:
                if file.endswith(".proto"):
                    proto_file = os.path.join(root, file)
                    self.run(f'"{protoc_exec}" --cpp_out="{os.path.join(self.source_folder,"stonex-cms-amqp","Test","schema")}" --proto_path="{os.path.join(self.source_folder,"stonex-cms-amqp","Test","schema")}"  "{proto_file}"')

        cmake = CMake(self)
        cmake.definitions["BUILD_TEST"] = "ON"
        cmake.verbose = True

    # Add MSVC MAP file generation
        if self.settings.compiler == "Visual Studio" or self.settings.get_safe("compiler") == "msvc":
            cmake.definitions["CMAKE_EXE_LINKER_FLAGS"] = "/MAP"
            cmake.definitions["CMAKE_SHARED_LINKER_FLAGS"] = "/MAP"
            cmake.definitions["CMAKE_MODULE_LINKER_FLAGS"] = "/MAP"
    
            cmake.configure(source_folder="stonex-cms-amqp")
            cmake.build()

    def package(self):
        self.copy("activemq-cpp\src\main\cms\*", dst="include",src="stonex-cms-amqp\stonex-cms-amqp-lib",keep_path=True)
        self.copy("*.h", dst="include",src="stonex-cms-amqp\stonex-cms-amqp-lib\stonex-cms-amqp-cpp",keep_path=True)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.map", dst="lib", keep_path=False)
        self.copy("*.pdb", dst="bin", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)


    def package_info(self):
        self.cpp_info.includedirs.append("include/activemq-cpp/src/main")
        self.cpp_info.system_libs.append("dbghelp")
        self.cpp_info.libs = self.collect_libs()
