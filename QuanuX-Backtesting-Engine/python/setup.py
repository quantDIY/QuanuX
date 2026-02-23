import os
from setuptools import setup, Extension

try:
    from Cython.Build import cythonize
    import numpy
except ImportError:
    cythonize = None
    numpy = None

current_dir = os.path.dirname(os.path.abspath(__file__))
cpp_dir = os.path.abspath(os.path.join(current_dir, "../cpp"))
build_dir = os.path.join(cpp_dir, "build")

# Gather static libraries built by CMake
extra_objects = [
    os.path.join(build_dir, "libbacktest_engine.a"),
    os.path.join(build_dir, "lib/libnats_static.a"),
    os.path.join(build_dir, "lib/Release/libdatabento.a"),
    os.path.join(build_dir, "_deps/duckdb-build/src/libduckdb_static.a"),
    os.path.join(build_dir, "_deps/duckdb-build/extension/parquet/libparquet_extension.a"),
    os.path.join(build_dir, "_deps/duckdb-build/extension/json/libjson_extension.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/yyjson/libduckdb_yyjson.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/fastpforlib/libduckdb_fastpforlib.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/fmt/libduckdb_fmt.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/fsst/libduckdb_fsst.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/libpg_query/libduckdb_pg_query.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/utf8proc/libduckdb_utf8proc.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/mbedtls/libduckdb_mbedtls.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/miniz/libduckdb_miniz.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/hyperloglog/libduckdb_hyperloglog.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/skiplist/libduckdb_skiplistlib.a"),
    os.path.join(build_dir, "_deps/duckdb-build/third_party/re2/libduckdb_re2.a"),
]

# Verify libraries exist
valid_objects = []
for obj in extra_objects:
    if os.path.exists(obj):
        valid_objects.append(obj)
    else:
        print(f"Warning: Static library not found: {obj}")

ext_modules = []

if cythonize:
    extensions = [
        Extension(
            "quanux_backtest.cython.quanux_crucible",
            ["quanux_backtest/cython/quanux_crucible.pyx"],
            language="c++",
            include_dirs=[
                os.path.join(cpp_dir, "include"),
                os.path.abspath(os.path.join(cpp_dir, "../../QuanuX-Common/cpp/include")),
                os.path.join(build_dir, "_deps/duckdb-src/src/include"),
                os.path.join(build_dir, "_deps/cnats-src/src"),
                os.path.join(build_dir, "_deps/databento-src/include"),
                numpy.get_include() if numpy else ""
            ],
            extra_objects=valid_objects,
            extra_compile_args=["-std=c++20", "-O3", "-undefined", "dynamic_lookup"],
        )
    ]
    ext_modules = cythonize(extensions, compiler_directives={'language_level': "3"})

setup(
    name="quanux_backtest",
    version="0.1.0",
    ext_modules=ext_modules,
    packages=["quanux_backtest"],
)
