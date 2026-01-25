#include "quanux/indicators/SMA.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace quanux::indicators;

PYBIND11_MODULE(quanux_indicators, m) {
  m.doc() = "Quanux Indicators (C++20 HFT Library)";

  py::class_<SMA>(m, "SMA")
      .def(py::init<int>())
      .def("update", &SMA::update, "Update the moving average with a new value")
      .def("value", &SMA::value, "Get the current value");
}
