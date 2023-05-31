import os

from conans import ConanFile, CMake, tools


class StonexCMSAMQPLibTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}  
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        # Current dir is "test_package/build/<build_id>" and CMakeLists.txt is
        # in "test_package"
        cmake.configure()
        cmake.build()
        
    def requirements(self):
        self.requires("stonex-cms-amqp-test-cases/1.1.0@enterprise_messaging/test")       
        self.requires("stonex-cms-amqp-test-framework/1.0.0@enterprise_messaging/test")
        self.requires("stonex-cms-amqp-test-engine/1.0.0@enterprise_messaging/test")
        self.requires("protobuf/3.20.1@enterprise_messaging/test")
        self.requires("fmt/9.1.0@enterprise_messaging/test")
        self.requires("boost/1.78.0@enterprise_messaging/stable")
        self.requires("stonex-logger-wrapper/0.0.2@enterprise_messaging/test")

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
