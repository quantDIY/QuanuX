cmake_minimum_required(VERSION 3.14)
project(tws_api_adapter)

set(CMAKE_CXX_STANDARD 17)

find_package(pybind11 REQUIRED)

# --- SDK INJECTION POINT ---
set(TWS_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sdks/twsapi")
if(NOT EXISTS "${TWS_SDK_DIR}")
    message(FATAL_ERROR "TWS API SDK not found at ${TWS_SDK_DIR}. Please run 'quanuxctl integrate tws_api' first.")
endif()

# Includes from injected SDK
include_directories(
    src/
    ${TWS_SDK_DIR}/IBJts/source/cppclient/client
)

# --- SOURCES ---
file(GLOB SOURCES "src/*.cpp")
# Add SDK sources to compilation unit if not built separately
file(GLOB SDK_SOURCES "${TWS_SDK_DIR}/IBJts/source/cppclient/client/*.cpp")

# --- PYTHON MODULE ---
pybind11_add_module(TwsApi SHARED ${SOURCES} ${SDK_SOURCES})

# Links
# TWS API usually needs pthread/socket stuff on linux/mac
if(UNIX)
    target_link_libraries(TwsApi PRIVATE pthread)
elseif(WIN32)
    target_link_libraries(TwsApi PRIVATE ws2_32)
endif()

message(STATUS "TWS API Adapter Configured. Wrappers will be built as TwsApi.so")
