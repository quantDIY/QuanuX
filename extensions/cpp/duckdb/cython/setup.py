import os
from setuptools import setup, Extension
from Cython.Build import cythonize

current_dir = os.path.dirname(os.path.abspath(__file__))
third_party_dir = os.path.abspath(os.path.join(current_dir, "../third_party"))

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

setup(
    name="duckdb_ext",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)
