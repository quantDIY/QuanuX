cmake_minimum_required(VERSION 3.20)
project(quanux-envoy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Basic optimization flags for a release build
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wall -Wextra")

# Locate Threads
find_package(Threads REQUIRED)

# Try finding spdlog, and CURL using find_package
# We use explicit find for NATS since Brew config might be missing
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

# Explicitly map NATS since find_package is brittle on macOS homebrew without natsConfig.cmake
find_path(NATS_INCLUDE_DIR nats/nats.h
  PATHS /usr/local/include /opt/homebrew/include /usr/include
)

find_library(NATS_LIBRARY NAMES nats nats_static
  PATHS /usr/local/lib /opt/homebrew/lib /usr/lib
)

if (NOT NATS_INCLUDE_DIR OR NOT NATS_LIBRARY)
    message(FATAL_ERROR "Could not find explicit CNATS headers or libraries.")
endif()

# Create the source directories if they don't exist
# src/main.cpp
# src/probe.cpp

add_executable(quanux-envoy
    src/main.cpp
    src/probe.cpp
)

# Target include directories
target_include_directories(quanux-envoy PRIVATE src ${NATS_INCLUDE_DIR})

# Link the libraries
# We link pthread for pthread_setaffinity_np
target_link_libraries(quanux-envoy PRIVATE
    Threads::Threads
    ${NATS_LIBRARY}
    spdlog::spdlog
    pthread
    nlohmann_json::nlohmann_json
    CURL::libcurl
    OpenSSL::Crypto
)
