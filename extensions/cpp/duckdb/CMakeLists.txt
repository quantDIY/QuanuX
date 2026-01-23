cmake_minimum_required(VERSION 3.14)
project(quanux_duckdb_ext LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
include(FetchContent)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.11.1
)
FetchContent_MakeAvailable(pybind11)

# DuckDB
# We link against libduckdb. For simplicity in extension development, 
# we can try fetching the source or using a pre-built static lib.
# Fetching source can be heavy. Let's try fetching the source for now to ensure compatibility.
FetchContent_Declare(
    duckdb
    GIT_REPOSITORY https://github.com/duckdb/duckdb.git
    GIT_TAG v1.1.3
    GIT_SHALLOW TRUE
)
# Disable DuckDB's own build options that might conflict or slow things down
set(BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHELL OFF CACHE BOOL "" FORCE)
set(BUILD_EXTENSIONS "parquet;json" CACHE STRING "" FORCE) # Minimal extensions

FetchContent_MakeAvailable(duckdb)

# Create the Python extension module
pybind11_add_module(duckdb_py src/bindings.cpp)

# Link against DuckDB
target_link_libraries(duckdb_py PRIVATE duckdb_static)

# Platform specific flags
if(APPLE)
    target_link_options(duckdb_py PRIVATE "-undefined" "dynamic_lookup")
endif()

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
