from setuptools import setup, Extension
from Cython.Build import cythonize
import os

orchestra_include = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "include"))
omega_include = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "omega", "include"))
omega_src = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "omega", "src", "omega", "translation", "decoder_ring.cpp"))

extensions = [
    Extension(
        "orchestra_bindings",
        ["orchestra_bindings.pyx"],
        include_dirs=[orchestra_include],
        language="c++",
        extra_compile_args=["-std=c++20", "-O3"]
    ),
    Extension(
        "omega_decoder",
        ["omega_decoder.pyx", omega_src],
        include_dirs=[omega_include, orchestra_include],
        language="c++",
        extra_compile_args=["-std=c++20", "-O3"]
    )
]

setup(
    name="quanux_orchestra",
    ext_modules=cythonize(extensions, language_level=3)
)
