cmake_minimum_required(VERSION 3.14)
project(ibkr_fix_adapter)

set(CMAKE_CXX_STANDARD 17)

# --- TURBO BUILD OPTIONS ---
option(QUANUX_TURBO "Enable High-Performance Optimizations" OFF)
set(QUANUX_ALLOCATOR "system" CACHE STRING "Memory Allocator (system|jemalloc|mimalloc)")
set(QUANUX_LOGGER "file" CACHE STRING "Logging Strategy (file|async|null)")

# QuickFIX Engine (Standard or Turbo enhanced)
# Manual configuration since local build doesn't export CMake config
set(QUICKFIX_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../deps/quickfix")
set(QUICKFIX_INCLUDE_DIRS "${QUICKFIX_ROOT}/include")
set(QUICKFIX_LIBRARIES "${QUICKFIX_ROOT}/lib/libquickfix.dylib")

if(NOT EXISTS "${QUICKFIX_INCLUDE_DIRS}")
    message(FATAL_ERROR "QuickFIX headers not found at ${QUICKFIX_INCLUDE_DIRS}. Please run the QuickFIX build workflow.")
endif()

# --- USER LIBS HOOK ---
# Allows users to inject their own math/algo libraries via `user_libs.cmake`
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/user_libs.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/user_libs.cmake")
    message(STATUS "Loaded user_libs.cmake")
endif()

# --- SOURCES ---
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

add_library(ibkr_fix_adapter SHARED ${SOURCES} ${HEADERS})

target_include_directories(ibkr_fix_adapter PUBLIC 
    src/
    ${QUICKFIX_INCLUDE_DIRS}
)

target_link_libraries(ibkr_fix_adapter PUBLIC ${QUICKFIX_LIBRARIES})

if(WIN32)
    target_link_libraries(ibkr_fix_adapter PRIVATE ws2_32)
endif()

# --- ALLOCATOR LINKING ---
if(QUANUX_ALLOCATOR STREQUAL "jemalloc")
    find_package(JeMalloc REQUIRED)
    target_link_libraries(ibkr_fix_adapter PRIVATE JeMalloc::JeMalloc)
    add_definitions(-DQUANUX_USE_JEMALLOC)
    message(STATUS "Turbo Mode: Linked against jemalloc")
elseif(QUANUX_ALLOCATOR STREQUAL "mimalloc")
    find_package(mimalloc REQUIRED)
    target_link_libraries(ibkr_fix_adapter PRIVATE mimalloc)
    add_definitions(-DQUANUX_USE_MIMALLOC)
    message(STATUS "Turbo Mode: Linked against mimalloc")
endif()

# --- LOGGER CONFIG ---
if(QUANUX_LOGGER STREQUAL "async")
    add_definitions(-DQUANUX_LOG_ASYNC)
    find_package(spdlog REQUIRED)
    target_link_libraries(ibkr_fix_adapter PRIVATE spdlog::spdlog)
elseif(QUANUX_LOGGER STREQUAL "null")
    add_definitions(-DQUANUX_LOG_NULL)
endif()

message(STATUS "IBKR FIX Adapter Configured. Allocator: ${QUANUX_ALLOCATOR}, Logger: ${QUANUX_LOGGER}")
