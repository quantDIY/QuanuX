cmake_minimum_required(VERSION 3.14)
project(tws_api_adapter)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11
  GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

# --- SDK INJECTION POINT ---
set(TWS_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sdks/twsapi")

# --- SOURCES ---
file(GLOB SOURCES "src/*.cpp")
set(SDK_SOURCES "")

if(EXISTS "${TWS_SDK_DIR}/IBJts/source/cppclient/client/EWrapper.h")
    message(STATUS "TWS API SDK Headers found at ${TWS_SDK_DIR}")
    include_directories(${TWS_SDK_DIR}/IBJts/source/cppclient/client)
    file(GLOB SDK_SOURCES "${TWS_SDK_DIR}/IBJts/source/cppclient/client/*.cpp")
    add_definitions(-DTWS_SDK_AVAIL)
else()
    message(WARNING "TWS API SDK Headers not found at ${TWS_SDK_DIR}. Building in MOCK mode.")
endif()

# Includes
include_directories(src/)

# --- PYTHON MODULE ---
pybind11_add_module(TwsApi SHARED ${SOURCES} ${SDK_SOURCES})

# Links
if(UNIX)
    target_link_libraries(TwsApi PRIVATE pthread)
elseif(WIN32)
    target_link_libraries(TwsApi PRIVATE ws2_32)
endif()

message(STATUS "TWS API Adapter Configured.")
