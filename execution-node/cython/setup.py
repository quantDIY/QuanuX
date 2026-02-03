from setuptools import setup, Extension
from Cython.Build import cythonize
import numpy

extensions = [
    Extension(
        "adapter", 
        sources=["src/adapter.pyx"],
        language="c++",
        include_dirs=[numpy.get_include()]
    ),
    Extension(
        "core",
        sources=["src/core.pyx"],
        language="c++",
        include_dirs=[numpy.get_include()]
    ),
    Extension(
        "direct_adapter",
        sources=["src/direct_adapter.pyx"],
        language="c++",
        include_dirs=[numpy.get_include()]
    ),
    Extension(
        "nats_adapter",
        sources=["src/nats_adapter.pyx"],
        language="c++",
        include_dirs=[numpy.get_include()]
    )
]

setup(
    name="quanux_node",
    ext_modules=cythonize(extensions, language_level=3)
)
