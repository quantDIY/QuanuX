import os
from setuptools import setup, Extension
try:
    from Cython.Build import cythonize
except ImportError:
    cythonize = None

# Define paths
current_dir = os.path.dirname(os.path.abspath(__file__))
include_dir = os.path.abspath(os.path.join(current_dir, "../include"))

ext_modules = []

if cythonize:
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
    ext_modules = cythonize(extensions, compiler_directives={'language_level': "3"})
else:
    print("Cython not found, skipping extension build")

setup(
    name="quanux_indicators",
    ext_modules=ext_modules,
)
