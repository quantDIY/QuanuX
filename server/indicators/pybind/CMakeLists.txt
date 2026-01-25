
# Try to find pybind11 via python if standard find fails
execute_process(
    COMMAND "${PYTHON_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(PYBIND11_CMAKE_DIR)
    list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
endif()

find_package(pybind11 REQUIRED)

pybind11_add_module(quanux_indicators bindings.cpp)

target_link_libraries(quanux_indicators PRIVATE quanux_indicators_lib)

# Initializer for the module to be used as a package
# We might need to copy the generated .so to the python package dir
