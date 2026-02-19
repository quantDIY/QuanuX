from setuptools import setup, Extension
try:
    from Cython.Build import cythonize
    import numpy
except ImportError:
    cythonize = None
    numpy = None

ext_modules = []

if cythonize and numpy:
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
    ext_modules = cythonize(extensions, language_level=3)
else:
    print("Cython or Numpy not found, skipping extension build")

setup(
    name="quanux_node",
    ext_modules=ext_modules
)
