cmake_minimum_required(VERSION 3.14)
project(QuanuX-Annex VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# DIRECTIVE: NO JVM ALLOWED. Pure C++17/20 Only.

set(CLOUD_TARGET "NONE" CACHE STRING "Target cloud environment: DO, AWS, GCP, AZURE")

if(CLOUD_TARGET STREQUAL "NONE")
    message(FATAL_ERROR "You must specify a CLOUD_TARGET (e.g., -DCLOUD_TARGET=AWS)")
endif()

message(STATUS "Configuring QuanuX-Annex Storage Abstraction Layer for cloud target: ${CLOUD_TARGET}")

include_directories(${CMAKE_SOURCE_DIR}/include)

set(RESOLVER_SOURCES "")

if(CLOUD_TARGET STREQUAL "DO")
    add_compile_definitions(CLOUD_TARGET_DO)
    set(RESOLVER_SOURCES src/resolvers/ZarrResolver_DO.cpp src/resolvers/ZarrRamFlusher.cpp src/resolvers/NatsSubscriber.cpp)
elseif(CLOUD_TARGET STREQUAL "AWS")
    set(RESOLVER_SOURCES src/resolvers/FSxResolver_AWS.cpp)
elseif(CLOUD_TARGET STREQUAL "GCP")
    set(RESOLVER_SOURCES src/resolvers/BigQuery_HDF5Resolver_GCP.cpp)
elseif(CLOUD_TARGET STREQUAL "AZURE")
    set(RESOLVER_SOURCES src/resolvers/Blob_HDF5Resolver_AZURE.cpp)
else()
    message(FATAL_ERROR "Invalid CLOUD_TARGET: ${CLOUD_TARGET}. Must be DO, AWS, GCP, or AZURE.")
endif()

# Library for Storage Resolvers
add_library(quanux_annex_resolvers STATIC ${RESOLVER_SOURCES})
target_include_directories(quanux_annex_resolvers PUBLIC include)

# Main Executable
find_package(Threads REQUIRED)

find_library(NATS_LIBRARY nats)
if (NOT NATS_LIBRARY)
    set(NATS_LIBRARY "nats") # Fallback to generic -lnats flag
endif()

add_executable(quanux_annex src/main.cpp src/federation/HasuraRemoteWorker.cpp)
target_include_directories(quanux_annex PUBLIC include)
target_link_libraries(quanux_annex PRIVATE quanux_annex_resolvers Threads::Threads ${NATS_LIBRARY})
