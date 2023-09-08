import os

from conans import ConanFile, CMake, tools


class StonexCMSAMQPLibTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "core_tests":[True, False]}
    default_options = {"shared": False, "fPIC": True, "core_tests": True}  
    generators = "cmake"

    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        if self.settings.arch == "x86":
            self.options["stonex-cms-amqp-test-framework"].use_32_time_t = True
            self.options["protobuf"].use_32_time_t = True

    def build(self):
        cmake = CMake(self)
        # Current dir is "test_package/build/<build_id>" and CMakeLists.txt is
        # in "test_package"
        cmake.configure()
        cmake.build()
        
    def requirements(self):
        self.requires("stonex-cms-amqp-test-cases/1.1.0@enterprise_messaging/test")       
        self.requires("stonex-cms-amqp-test-framework/1.2.0@enterprise_messaging/test")
        self.requires("stonex-cms-amqp-test-engine/2.0.1@enterprise_messaging/test")
        self.requires("protobuf/3.20.1@enterprise_messaging/test")
        self.requires("boost/1.78.0@enterprise_messaging/stable")

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def test(self): 
        os.chdir("bin")
        ext = ('.config')
        for files in os.listdir():
            if files.endswith(ext):
                self.run(".%sstonex-cms-amqp-tests.exe %s"  % (os.sep, files))
