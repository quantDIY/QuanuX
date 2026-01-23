cmake_minimum_required(VERSION 3.24)
project(databento_py)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DATABENTO_ENABLE_TESTING OFF)
set(DATABENTO_ENABLE_EXAMPLES OFF)

include(FetchContent)

# Pybind11
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

# DuckDB for Connector
FetchContent_Declare(
    duckdb
    GIT_REPOSITORY https://github.com/duckdb/duckdb.git
    GIT_TAG v1.1.3
    GIT_SHALLOW TRUE
)
set(BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHELL OFF CACHE BOOL "" FORCE)
set(BUILD_EXTENSIONS "parquet;json" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(duckdb)

# JSON (Dependency of Databento)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# Databento CPP
FetchContent_Declare(
  databento
  GIT_REPOSITORY https://github.com/databento/databento-cpp.git
  GIT_TAG        v0.9.1
)
FetchContent_MakeAvailable(databento)

# Wrapper Module
pybind11_add_module(databento_py src/bindings.cpp)

# Link
target_link_libraries(databento_py PRIVATE databento::databento duckdb_static)

# Install/Copy artifact
add_custom_command(TARGET databento_py POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:databento_py> ${CMAKE_CURRENT_SOURCE_DIR}/../../python/wrappers/quanux_databento/
    COMMENT "Copying extension to python wrappers"
)
