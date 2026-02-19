import os
from setuptools import setup, Extension
try:
    from Cython.Build import cythonize
except ImportError:
    cythonize = None

current_dir = os.path.dirname(os.path.abspath(__file__))

# Include paths
# 1. shim is at ../shim
# 2. QuanuX/Common.hpp is at ../../common/include
shim_dir = os.path.abspath(os.path.join(current_dir, "../shim"))
common_dir = os.path.abspath(os.path.join(current_dir, "../../common/include"))

ext_modules = []

if cythonize:
    print(f"Build info:")
    print(f"  Shim Dir: {shim_dir}")
    print(f"  Common Dir: {common_dir}")

    extensions = [
        Extension(
            "tws_api",
            ["tws_api.pyx"],
            include_dirs=[shim_dir, common_dir],
            language="c++",
            extra_compile_args=["-std=c++17", "-O3", "-undefined", "dynamic_lookup"]
        )
    ]
    ext_modules = cythonize(extensions, compiler_directives={'language_level': "3"})
else:
    print("Cython not found, skipping extension build")

setup(
    name="tws_api",
    ext_modules=ext_modules,
)
