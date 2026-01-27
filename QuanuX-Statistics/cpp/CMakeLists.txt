cmake_minimum_required(VERSION 3.20)
project(quanux_stats CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if (UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -fno-omit-frame-pointer")
endif()

# Dependencies
include(FetchContent)

# 1. NATS (Telemetry/Data)
set(NATS_BUILD_STREAMING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  cnats
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG        v3.9.0
)
FetchContent_MakeAvailable(cnats)

# 2. DuckDB (Storage/Analytics)
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

# 3. Eigen (Math/Stats)
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(eigen)

# 4. JSON
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Includes
include_directories(include)
include_directories(../../QuanuX-Common/cpp/include)

# Core Library
add_library(stats_core STATIC
    src/stats_engine.cpp
    src/time_series_store.cpp
)
target_link_libraries(stats_core PUBLIC 
    nats_static 
    duckdb_static 
    Eigen3::Eigen 
    nlohmann_json::nlohmann_json
    pthread 
    dl
)

# Main Service
add_executable(quanux_stats src/main.cpp)
target_link_libraries(quanux_stats PRIVATE stats_core)
