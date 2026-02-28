cmake_minimum_required(VERSION 3.20)
project(quanux-envoy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Basic optimization flags for a release build
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wall -Wextra")

# Locate Threads
find_package(Threads REQUIRED)

# Try finding spdlog, and CURL using find_package
find_package(spdlog QUIET)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

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

# Fetch CNATS securely
FetchContent_Declare(nats
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG v3.8.2
)
set(NATS_BUILD_TESTS OFF CACHE BOOL "Don't build NATS tests" FORCE)
set(NATS_BUILD_EXAMPLES OFF CACHE BOOL "Don't build NATS examples" FORCE)
set(NATS_BUILD_STREAMING OFF CACHE BOOL "Don't build streaming" FORCE)
set(NATS_BUILD_WITH_TLS OFF CACHE BOOL "Don't build TLS" FORCE)
set(NATS_BUILD_KV OFF CACHE BOOL "Don't build KV" FORCE)
set(NATS_BUILD_MICRO OFF CACHE BOOL "Don't build Micro" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "No Testing" FORCE)
FetchContent_MakeAvailable(nats)

FetchContent_GetProperties(nats)
if(NOT nats_POPULATED)
    FetchContent_Populate(nats)
endif()

# Create the source directories if they don't exist
# src/main.cpp
# src/probe.cpp

add_executable(quanux-envoy
    src/main.cpp
    src/probe.cpp
)

# Target include directories
target_include_directories(quanux-envoy PRIVATE 
    src
    ${nats_SOURCE_DIR}/src
)

# Link the libraries
# We link pthread for pthread_setaffinity_np
target_link_libraries(quanux-envoy PRIVATE
    Threads::Threads
    nats
    spdlog::spdlog
    pthread
    nlohmann_json::nlohmann_json
    CURL::libcurl
    OpenSSL::Crypto
)
