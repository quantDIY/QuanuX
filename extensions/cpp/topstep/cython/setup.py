from setuptools import setup, Extension
try:
    from Cython.Build import cythonize
except ImportError:
    cythonize = None

ext_modules = []

if cythonize:
    extensions = [
        Extension(
            "topstep_ext",
            ["topstep.pyx"],
            language="c++",
            extra_compile_args=["-std=c++17", "-O3"]
        )
    ]
    ext_modules = cythonize(extensions, compiler_directives={'language_level': "3"})
else:
    print("Cython not found, skipping extension build")

setup(
    name="topstep_ext",
    ext_modules=ext_modules,
)
