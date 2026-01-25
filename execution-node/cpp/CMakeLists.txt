cmake_minimum_required(VERSION 3.20)
project(quanux_node CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization Flags for HFT
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if (UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -fno-omit-frame-pointer")
endif()

# Dependencies
include(FetchContent)

# NATS.c (Telemetry)
set(NATS_BUILD_STREAMING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  cnats
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG        v3.9.0
)
FetchContent_MakeAvailable(cnats)

# DuckDB (For Backtester)
FetchContent_Declare(
    duckdb
    GIT_REPOSITORY https://github.com/duckdb/duckdb.git
    GIT_TAG v1.1.3
    GIT_SHALLOW TRUE
)
set(BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHELL OFF CACHE BOOL "" FORCE)
# Build parquet extension statically for zero-copy arrow support
set(BUILD_EXTENSIONS "parquet;json" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(duckdb)

# Include Directories
include_directories(include)

# Indicators Library (Self-Contained Strategy Support)
# We pull this in so strategies can compute locally without phoning home.
set(BUILD_PYTHON_BINDINGS OFF CACHE BOOL "" FORCE)
add_subdirectory(../../server/indicators ${CMAKE_BINARY_DIR}/indicators)

# Core Library (The Engine)
add_library(node_core STATIC
    src/engine.cpp
    src/nats_bridge.cpp
    src/market_data_engine.cpp
    src/order_gateway.cpp
)
target_link_libraries(node_core PUBLIC nats_static pthread dl)

# Main Executable
add_executable(quanux_node src/main.cpp)
target_link_libraries(quanux_node PRIVATE node_core)

# Simulator
add_library(node_simulator STATIC
    src/simulator/SimulatedExchange.cpp
)
target_link_libraries(node_simulator PUBLIC node_core duckdb_static)

add_executable(quanux_backtest src/simulator/main.cpp)
target_link_libraries(quanux_backtest PRIVATE node_simulator)

# Strategies
add_library(ping_pong SHARED strategies/ping_pong.cpp)
target_include_directories(ping_pong PRIVATE include)
set_target_properties(ping_pong PROPERTIES PREFIX "") # ping_pong.so

# Mock Feed
add_library(mock_feed SHARED strategies/mock_feed.cpp)
target_include_directories(mock_feed PRIVATE include)
set_target_properties(mock_feed PROPERTIES PREFIX "") # mock_feed.so

# Demo Strategy (Indicator Verification)
add_library(demo_strategy SHARED strategies/demo_strategy.cpp)
target_include_directories(demo_strategy PRIVATE include)
# LINK against the indicators library we imported!
target_link_libraries(demo_strategy PRIVATE quanux_indicators_lib)
set_target_properties(demo_strategy PROPERTIES PREFIX "") # demo_strategy.so

# Strategy Interface (Header Only for now)
install(FILES include/strategy_interface.h include/plugin_loader.h include/ring_buffer.h include/market_data_engine.h include/nats_bridge.h include/engine.h include/order_gateway.h include/risk_engine.h DESTINATION include)
