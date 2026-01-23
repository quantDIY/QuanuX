cmake_minimum_required(VERSION 3.15)
project(rithmic_feed CXX)

set(CMAKE_CXX_STANDARD 17) # Rithmic might not support 20 fully, safe bet 17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Rithmic SDK Paths
set(RITHMIC_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdk")
set(RITHMIC_INC_DIR "${RITHMIC_SDK_DIR}/include")
# Platform Detection
if(APPLE)
    # macOS
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
        set(RITHMIC_LIB_DIR "${RITHMIC_SDK_DIR}/darwin-20.6-arm64/lib")
        message(STATUS "Detected Apple Silicon - using darwin-20.6-arm64 SDK")
    else()
        set(RITHMIC_LIB_DIR "${RITHMIC_SDK_DIR}/darwin-10/lib") 
        message(STATUS "Detected Intel Mac - using darwin-10 SDK")
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux (x86_64) - Check uname -m in future if supporting ARM linux
    set(RITHMIC_LIB_DIR "${RITHMIC_SDK_DIR}/linux-gnu-3.10.0-x86_64/lib")
endif()

# Include directories
include_directories(${RITHMIC_INC_DIR})

# --- Executable Build (Standalone) ---
file(GLOB SOURCES "src/main.cpp")
add_executable(rithmic_feed ${SOURCES})

# Find System Libraries
find_package(ZLIB REQUIRED) # Rithmic likely needs zlib

# Link libraries for Executable
# Order matters: dependent libs first
target_link_libraries(rithmic_feed PRIVATE
    ${RITHMIC_LIB_DIR}/libRApiPlus-optimize.a
    ${RITHMIC_LIB_DIR}/libOmneEngine-optimize.a
    ${RITHMIC_LIB_DIR}/libOmneStreamEngine-optimize.a
    ${RITHMIC_LIB_DIR}/libOmneChannel-optimize.a
    ${RITHMIC_LIB_DIR}/lib_api-optimize.a
    ${RITHMIC_LIB_DIR}/lib_kit-optimize.a
    ${RITHMIC_LIB_DIR}/libssl.a
    ${RITHMIC_LIB_DIR}/libcrypto.a
    ZLIB::ZLIB
    pthread
)

if(APPLE)
    target_link_libraries(rithmic_feed PRIVATE
        "-framework CoreFoundation"
        "-framework SystemConfiguration"
        "-framework Security"
    )
endif()

# --- Python Module Build (Pybind11) ---
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11
  GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(rithmic_py "src/bindings.cpp")

target_link_libraries(rithmic_py PRIVATE
    ${RITHMIC_LIB_DIR}/libRApiPlus-optimize.a
    ${RITHMIC_LIB_DIR}/libOmneEngine-optimize.a
    ${RITHMIC_LIB_DIR}/libOmneStreamEngine-optimize.a
    ${RITHMIC_LIB_DIR}/libOmneChannel-optimize.a
    ${RITHMIC_LIB_DIR}/lib_api-optimize.a
    ${RITHMIC_LIB_DIR}/lib_apipoll-stubs-optimize.a
    ${RITHMIC_LIB_DIR}/lib_kit-optimize.a
    ${RITHMIC_LIB_DIR}/libssl.a
    ${RITHMIC_LIB_DIR}/libcrypto.a
    ZLIB::ZLIB
    # Excluding pthread/frameworks here? pybind11 usually handles basic linking, but Rithmic needs these.
)

if(APPLE)
    target_link_libraries(rithmic_py PRIVATE
        "-framework CoreFoundation"
        "-framework SystemConfiguration"
        "-framework Security"
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(rithmic_py PRIVATE pthread dl)
endif()
