import os
import sys
from setuptools import setup, Extension
from Cython.Build import cythonize

current_dir = os.path.dirname(os.path.abspath(__file__))
# Assume CMake build is at ../build
build_dir = os.path.abspath(os.path.join(current_dir, "../build"))
include_dir = os.path.join(build_dir, "_deps/databento-src/include")
lib_dir = os.path.join(build_dir, "lib/Release")

# If headers don't exist, we can't build.
# We will assume user runs cmake first.

httplib_dir = os.path.join(build_dir, "_deps/httplib-src")
json_dir = os.path.join(build_dir, "_deps/json-src/include")

extensions = [
    Extension(
        "databento_ext",
        ["databento.pyx"],
        include_dirs=[include_dir, httplib_dir, json_dir],
        library_dirs=[lib_dir, "/usr/local/lib"],
        libraries=["databento", "zstd", "curl", "ssl", "crypto", "brotlicommon", "brotlidec", "brotlienc"],
        language="c++",
        extra_compile_args=["-std=c++17", "-O3", "-undefined", "dynamic_lookup"]
    )
]

setup(
    name="databento_ext",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)
