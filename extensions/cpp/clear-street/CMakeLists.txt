cmake_minimum_required(VERSION 3.20)
project(quanux_clear_street_adapter CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Dependencies ---
include(FetchContent)

# 1. OnixS (Proprietary / Injected)
set(ONIXS_ROOT "../../sdks/onixs")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ONIXS_ROOT}/include/OnixS/FixEngine.h")
    message(STATUS "Found OnixS SDK at ${ONIXS_ROOT}")
    set(USE_ONIXS ON)
    add_definitions(-DUSE_ONIXS)
    include_directories(${ONIXS_ROOT}/include)
    link_directories(${ONIXS_ROOT}/lib)
else()
    message(STATUS "OnixS SDK not found. Defaulting to QuickFIX only.")
    set(USE_ONIXS OFF)
endif()

# 2. QuickFIX (Open Source Fallback)
FetchContent_Declare(
    quickfix
    GIT_REPOSITORY https://github.com/quickfix/quickfix.git
    GIT_TAG v1.15.1
)
set(HAVE_SSL OFF CACHE BOOL "" FORCE) # Disable SSL to avoid OpenSSL dependency issues in minimal env
FetchContent_GetProperties(quickfix)
if(NOT quickfix_POPULATED)
    FetchContent_Populate(quickfix)
    # Fix for QuickFIX assuming CMAKE_MODULE_PATH is local
    list(APPEND CMAKE_MODULE_PATH "${quickfix_SOURCE_DIR}/cmake")

    # Patch QuickFIX to disable examples and tests (which fail to build in this env)
    file(READ "${quickfix_SOURCE_DIR}/CMakeLists.txt" QF_CMAKE)
    string(REPLACE "add_subdirectory(examples)" "# add_subdirectory(examples)" QF_CMAKE "${QF_CMAKE}")
    string(REPLACE "add_subdirectory(test)" "# add_subdirectory(test)" QF_CMAKE "${QF_CMAKE}")
    string(REPLACE "add_subdirectory(UnitTest++)" "# add_subdirectory(UnitTest++)" QF_CMAKE "${QF_CMAKE}")
    file(WRITE "${quickfix_SOURCE_DIR}/CMakeLists.txt" "${QF_CMAKE}")

    add_subdirectory(${quickfix_SOURCE_DIR} ${quickfix_BINARY_DIR})
    # Downgrade QuickFIX to C++14 to avoid C++17 removal of dynamic exception specs
    set_target_properties(quickfix PROPERTIES CXX_STANDARD 14)
    # Fix for double-conversion include path (header not found error)
    target_include_directories(quickfix PRIVATE "${quickfix_SOURCE_DIR}/src/C++")
endif()
# Remove global include to prevent CNATS picking up QuickFIX headers
# include_directories(${quickfix_SOURCE_DIR}/src/C++)
set(QuickFIX_LIBRARIES quickfix)

# 3. DuckDB (Persistence)
FetchContent_Declare(
    duckdb
    GIT_REPOSITORY https://github.com/duckdb/duckdb.git
    GIT_TAG v1.1.3
    GIT_SHALLOW TRUE
)
set(BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHELL OFF CACHE BOOL "" FORCE)
set(BUILD_EXTENSIONS "parquet" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(duckdb)

# 4. NATS C (Messaging)
set(NATS_BUILD_STREAMING OFF CACHE BOOL "" FORCE)
set(NATS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(NATS_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  cnats
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG        v3.9.0
)
FetchContent_MakeAvailable(cnats)

# --- Sources ---

set(SOURCES
    src/ClearStreetAdapter.cpp
    src/Metrics/LatencyTracker.cpp
    src/Risk/RiskEngine.cpp
    src/Recorder/MarketRecorder.cpp
    src/Data/DataArchiver.cpp
    src/Bus/MessagePublisher.cpp
    src/Strategy/StrategyLoader.cpp
)

if(USE_ONIXS)
    list(APPEND SOURCES src/Backends/OnixSBackend.cpp)
endif()

# Bridge Library (C++14 for QuickFIX Legacy Headers)
add_library(quickfix_bridge STATIC src/Backends/QuickFIXFactory.cpp)
set_target_properties(quickfix_bridge PROPERTIES CXX_STANDARD 14)
target_include_directories(quickfix_bridge PRIVATE "${quickfix_SOURCE_DIR}/src/C++")
target_link_libraries(quickfix_bridge PRIVATE ${QuickFIX_LIBRARIES} duckdb_static nats_static)

# Primary Library (C++20)
add_library(clear_street_lib STATIC ${SOURCES})
target_include_directories(clear_street_lib PRIVATE "${quickfix_SOURCE_DIR}/src/C++")
target_link_libraries(clear_street_lib PUBLIC duckdb_static nats_static ${QuickFIX_LIBRARIES} dl quickfix_bridge)

if(USE_ONIXS)
    # Link against OnixS libs (names may vary by distro, adjusting for generic Linux/macOS)
    # Assuming dynamic linking for OnixS
    target_link_libraries(clear_street_lib PUBLIC onixs_fix_engine) 
endif()

# Main Adapter Executable (Standalone Runner)
add_executable(quanux_custom_adapter src/main.cpp)
target_link_libraries(quanux_custom_adapter PRIVATE clear_street_lib)

# Tests
# Tests
enable_testing()

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
# Prevent GTest from overriding our compiler/linker options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_clear_street tests/test_clear_street_adapter.cpp)
target_link_libraries(test_clear_street PRIVATE clear_street_lib gtest_main)
