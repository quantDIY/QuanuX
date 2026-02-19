import os
from setuptools import setup, Extension
try:
    from Cython.Build import cythonize
except ImportError:
    cythonize = None

current_dir = os.path.dirname(os.path.abspath(__file__))
third_party_dir = os.path.abspath(os.path.join(current_dir, "../third_party"))

ext_modules = []

if cythonize:
    print(f"Build info:")
    print(f"  Third Party: {third_party_dir}")

    extensions = [
        Extension(
            "duckdb_ext",
            ["duckdb_ext.pyx", os.path.join(third_party_dir, "duckdb.cpp")],
            include_dirs=[third_party_dir],
            language="c++",
            extra_compile_args=["-std=c++17", "-O3", "-undefined", "dynamic_lookup", "-DDUCKDB_STATIC_BUILD"] 
        )
    ]
    ext_modules = cythonize(extensions, compiler_directives={'language_level': "3"})
else:
    print("Cython not found, skipping extension build")

setup(
    name="duckdb_ext",
    ext_modules=ext_modules,
)
