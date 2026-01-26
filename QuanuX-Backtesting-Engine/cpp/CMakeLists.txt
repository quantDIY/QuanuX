cmake_minimum_required(VERSION 3.20)
project(quanux_backtest_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes
include_directories(include)
include_directories(../../QuanuX-Common/cpp/include)

# Dependencies
include(FetchContent)

# DuckDB
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

# NATS (For Market Replay)
set(NATS_BUILD_STREAMING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  cnats
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG        v3.9.0
)
FetchContent_MakeAvailable(cnats)

# JSON (Dependency of Databento, explicit override for CMake 3.20+ compact)
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

# Core Backtest Engine Library
add_library(backtest_engine STATIC
    src/engine/BacktestRunner.cpp 
    src/engine/NatsReplayer.cpp
    src/engine/DbnPipeFeeder.cpp
)
target_link_libraries(backtest_engine PUBLIC duckdb_static nats_static databento::databento)

# Main Executable
add_executable(quanux_backtest src/main.cpp)
target_link_libraries(quanux_backtest PRIVATE backtest_engine duckdb_static nats_static pthread dl)


# Metrics Test
add_executable(test_metrics tests/test_metrics.cpp)
target_link_libraries(test_metrics PRIVATE backtest_engine)

# Pybind11 (Python Bindings)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11
  GIT_TAG        v2.12.0
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(quanux_metrics src/bindings/bindings.cpp)
# PerformanceAnalyzer is header-only, so no need to link backtest_engine lib if not using other parts.
# But we include the directories implicitly via target linkage or include_directories above.
target_link_libraries(quanux_metrics PRIVATE pybind11::module)
