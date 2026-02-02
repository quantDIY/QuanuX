import os
from setuptools import setup, Extension
from Cython.Build import cythonize

# Define paths
current_dir = os.path.dirname(os.path.abspath(__file__))
include_dir = os.path.abspath(os.path.join(current_dir, "../include"))

print(f"Build info:")
print(f"  Current: {current_dir}")
print(f"  Include: {include_dir}")

extensions = [
    Extension(
        "quanux_indicators",
        ["quanux_indicators.pyx"],
        include_dirs=[include_dir],
        language="c++",
        extra_compile_args=["-std=c++20", "-O3", "-undefined", "dynamic_lookup"] # C++20 required
    )
]

setup(
    name="quanux_indicators",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)
