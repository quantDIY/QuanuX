from setuptools import setup, Extension
from Cython.Build import cythonize

ext_modules = [
    Extension(
        "lookback_bridge",
        sources=["lookback_bridge.pyx", "lookback_engine.cpp"],
        language="c++",
        extra_compile_args=["-std=c++11", "-O3"]
    )
]

setup(
    name="quanux_lookback_bridge",
    ext_modules=cythonize(ext_modules)
)
