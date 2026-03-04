from setuptools import setup
from Cython.Build import cythonize

setup(
    ext_modules=cythonize(
        "telemetry_compiler.pyx", 
        compiler_directives={
            'language_level': "3",
            'boundscheck': False,
            'wraparound': False,
            'nonecheck': False,
            'c_string_type': 'unicode',
            'c_string_encoding': 'utf8'
        }
    )
)
