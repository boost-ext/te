import os

from conans import ConanFile, tools


class TE(ConanFile):
    name = "TE"
    version = "latest"
    url = "https://github.com/boost-ext/te"
    license = "Boost"
    description = "[Boost::ext].TE: C++17 Type Erasure Library"
    settings = "os", "compiler", "arch", "build_type"
    exports_sources = "include/*"
    no_copy_source = True

    def package(self):
        self.copy("*.hpp")


