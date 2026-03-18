from setuptools import setup, Extension
from Cython.Build import cythonize

ext_modules = [
    Extension(
        "ingest_worker",
        sources=["ingest_worker.pyx", "worker_engine.cpp"],
        language="c++",
        extra_compile_args=["-std=c++11", "-O3"]
    )
]

setup(
    name="aleph_ingest_worker",
    ext_modules=cythonize(ext_modules)
)
