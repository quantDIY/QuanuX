from setuptools import setup, Extension
from Cython.Build import cythonize
import os

# Adjust path to where pybind11 headers are
pybind_include = os.path.abspath("../../server/indicators/build/_deps/pybind11-src/include")

print(f"Using pybind11 include: {pybind_include}")

pybind_ext = Extension(
    "bench_pybind",
    ["benchmark_pybind.cpp"],
    include_dirs=[pybind_include],
    language="c++",
    extra_compile_args=["-std=c++17", "-O3", "-undefined", "dynamic_lookup"] # Mac specific linking
)

cython_ext = Extension(
    "bench_cython",
    ["benchmark_cython.pyx"],
    extra_compile_args=["-O3"]
)

setup(
    name="benchmarks",
    ext_modules=cythonize([cython_ext], compiler_directives={'language_level': "3"}) + [pybind_ext],
)
