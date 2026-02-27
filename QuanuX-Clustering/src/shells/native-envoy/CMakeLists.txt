cmake_minimum_required(VERSION 3.20)
project(quanux-envoy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Basic optimization flags for a release build
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wall -Wextra")

# Locate Threads
find_package(Threads REQUIRED)

# Try finding NATS and spdlog using find_package
find_package(nats QUIET)
find_package(spdlog QUIET)

# If not found via standard paths, we can define FetchContent for spdlog
include(FetchContent)
if(NOT spdlog_FOUND)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.12.0
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# Fetch nlohmann/json for secure IPC parsing
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Since libnats-c can be tricky on some systems without building, 
# for now we'll assume it's installed system-wide or we'll link against it directly.
# (The user instructed to use apt-get install libnats-dev in node_init.sh)

# Create the source directories if they don't exist
# src/main.cpp
# src/probe.cpp

add_executable(quanux-envoy
    src/main.cpp
    src/probe.cpp
)

# Target include directories
target_include_directories(quanux-envoy PRIVATE src)

# Link the libraries
# We link pthread for pthread_setaffinity_np
target_link_libraries(quanux-envoy PRIVATE
    Threads::Threads
    nats
    spdlog::spdlog
    pthread
    nlohmann_json::nlohmann_json
)
