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
FetchContent_Declare(
  cnats
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG        v3.9.0
)
FetchContent_MakeAvailable(cnats)

# Include Directories
include_directories(include)

# Core Library (The Engine)
add_library(node_core STATIC
    src/engine.cpp
    src/nats_bridge.cpp
)
target_link_libraries(node_core PUBLIC nats_static pthread dl)

# Main Executable
add_executable(quanux_node src/main.cpp)
target_link_libraries(quanux_node PRIVATE node_core)

# Strategy Interface (Header Only for now)
install(FILES include/strategy_interface.h DESTINATION include)
