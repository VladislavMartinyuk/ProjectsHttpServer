from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.cmake import CMake

class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("boost/1.90.0")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()