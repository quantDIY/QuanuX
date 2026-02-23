cmake_minimum_required(VERSION 3.20)
project(QuanuX-Spreader VERSION 1.0.0 LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# HFT Compilation Flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -DNDEBUG")

# Dependencies
include(FetchContent)

# cnats
set(NATS_BUILD_STREAMING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  cnats
  GIT_REPOSITORY https://github.com/nats-io/nats.c.git
  GIT_TAG        v3.9.0
)
FetchContent_MakeAvailable(cnats)

# Add include directories
include_directories(
    include
    ${CMAKE_SOURCE_DIR}/QuanuX-Common/cpp/include
)

# Injection Logic: Define STRATEGY_HEADER if provided
if(DEFINED STRATEGY_HEADER)
    add_compile_definitions(INJECTED_STRATEGY_HEADER="${STRATEGY_HEADER}")
    message(STATUS "Spreader compiling with injected strategy: ${STRATEGY_HEADER}")
else()
    # Default to the stub if no strategy is injected
    add_compile_definitions(INJECTED_STRATEGY_HEADER="spreader/strategy_stub.hpp")
    message(WARNING "No STRATEGY_HEADER provided. Compiling with default stub.")
endif()

# Executable target
add_executable(quanux_spreader
    src/main.cpp
    src/engine.cpp
)

# Link against any necessary libraries
find_package(Threads REQUIRED)
target_link_libraries(quanux_spreader PRIVATE Threads::Threads nats_static)
