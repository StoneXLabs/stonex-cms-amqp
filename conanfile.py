from conans import ConanFile, CMake, tools

def get_verion_tag():
    git = tools.Git("stonex-cms-amqp")
    try:
        return "%s" % (git.get_tag())
    except Exception as e:
        return e

class StonexCMSAMQPLib(ConanFile):
    name = "stonex-cms-amqp-lib"
    version = get_verion_tag()
    license = "Apache 2.0"
    author = "Krzysztof Obrebski krzysztof.obrebski@stonex.com"
    url = "https://github.com/StoneXLabs/stonex-cms-amqp.git"
    description = "amqp cms messaging library"
    topics = ("messaging", "c++", "cpp","proton","amqp","cms")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}  
    generators = "cmake"
    exports_sources = ["include/activemq-cpp/src/main/*"]

    def requirements(self):
        self.requires("red-hat-amq-client/2.10.4@enterprise_messaging/stable")
        self.requires("jsoncpp/1.9.5@enterprise_messaging/test")
        self.requires("stonex-logger-wrapper/1.0.0@enterprise_messaging/test")
        self.requires("fmt/9.1.0@enterprise_messaging/test")

    def build_requirements(self):
        self.build_requires("gtest/1.10.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        pass
        
    def build(self):
        cmake = CMake(self)
        cmake.definitions["CONAN_BUILD"] = "ON"
        cmake.definitions["BUILD_TEST"] = "ON"
        cmake.verbose = True
        cmake.configure(source_folder="stonex-cms-amqp")
        cmake.build()
		
          
        print("##teamcity[setParameter name='{}' value='{}']".format("stonex-cms-amqp-lib_version",get_verion_tag()))


    def package(self):
        self.copy("API\*.h", dst="include",src="stonex-cms-amqp\stonex-cms-amqp-lib")
        self.copy("activemq-cpp\src\main\cms\*", dst="include",src="stonex-cms-amqp\stonex-cms-amqp-lib",keep_path=True)
        self.copy("stonex-cms-amqp-lib.lib", dst="lib",src="lib", keep_path=False)
        self.copy("stonex-cms-amqp-lib.pdb", dst="lib",src="lib", keep_path=False)
        self.copy("*.dll", dst="bin",src="bin", keep_path=False)


    def package_info(self):
        self.cpp_info.includedirs.append("include/activemq-cpp/src/main")
        self.cpp_info.libs = self.collect_libs()