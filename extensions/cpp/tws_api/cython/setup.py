import os
from setuptools import setup, Extension
from Cython.Build import cythonize

current_dir = os.path.dirname(os.path.abspath(__file__))

# Include paths
# 1. shim is at ../shim
# 2. QuanuX/Common.hpp is at ../../common/include
shim_dir = os.path.abspath(os.path.join(current_dir, "../shim"))
common_dir = os.path.abspath(os.path.join(current_dir, "../../common/include"))

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

setup(
    name="tws_api",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)
