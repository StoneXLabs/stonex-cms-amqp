from conans import ConanFile, CMake, tools


class StonexCMSAMQExamples(ConanFile):
    name = "stonex-cms-amqp-examples"
    license = "Apache 2.0"
    author = "Krzysztof Obrebski krzysztof.obrebski@stonex.com"
    url = "https://github.com/StoneXLabs/stonex-cms-amqp.git"
    version = "0.0.1"
    description = "amqp cms messaging library"
    topics = ("messaging", "c++", "cpp","proton","amqp","cms")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}  
    generators = "cmake"
    exports_sources = ["include/activemq-cpp/src/main/*"]

    def requirements(self):
        self.requires("stonex-cms-amqp-lib/dev_0.3.4@enterprise_messaging/test")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        pass
        
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()