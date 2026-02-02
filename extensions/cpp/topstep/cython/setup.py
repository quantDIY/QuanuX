from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [
    Extension(
        "topstep_ext",
        ["topstep.pyx"],
        language="c++",
        extra_compile_args=["-std=c++17", "-O3"]
    )
]

setup(
    name="topstep_ext",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)
