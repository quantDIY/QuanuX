#include <pybind11/pybind11.h>

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(bench_pybind, m) {
    m.doc() = "pybind11 benchmark plugin";
    m.def("add", &add, "A function which adds two numbers");
}
