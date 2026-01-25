#include "quanux/indicators/market_profile.hpp"
#include "quanux/indicators/sma.hpp"
#include "quanux/indicators/volume_profile.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace quanux::indicators;

PYBIND11_MODULE(quanux_indicators, m) {
  m.doc() = "Quanux Indicators (C++20 HFT Library)";

  // -------------------------------------------------------------------------
  // 1. Standard Indicators (Eager Wrappers)
  // -------------------------------------------------------------------------

  // SMA
  // Usage: indicators.compute_sma([1.0, 2.0, ...], 10)
  m.def("compute_sma", &compute_sma<std::vector<double>>,
        "Compute Simple Moving Average", py::arg("data"), py::arg("period"));

  // -------------------------------------------------------------------------
  // 2. Market Profile
  // -------------------------------------------------------------------------

  // Dense Profile (Vector Based)
  using DenseProfile = MarketProfile<DenseStorage>;
  py::class_<DenseProfile>(m, "DenseMarketProfile")
      .def(py::init<double, double, int>(), py::arg("base_price"),
           py::arg("tick_size"), py::arg("initial_capacity") = 1000)
      .def("process", &DenseProfile::process, "Add a price tick")
      .def("query", &DenseProfile::query, "Get TPO count at price")
      .def("reset", &DenseProfile::reset, "Clear profile");

  // Sparse Profile (Map Based)
  using SparseProfile = MarketProfile<SparseStorage>;
  py::class_<SparseProfile>(m, "SparseMarketProfile")
      .def(py::init<double>(), py::arg("tick_size"))
      .def("process", &SparseProfile::process, "Add a price tick")
      .def("query", &SparseProfile::query, "Get TPO count at price")
      .def("reset", &SparseProfile::reset, "Clear profile");

  // -------------------------------------------------------------------------
  // 3. Volume Profile
  // -------------------------------------------------------------------------

  // Dense Volume Profile
  using DenseVolProfile = VolumeProfile<DenseStorage>;
  py::class_<DenseVolProfile>(m, "DenseVolumeProfile")
      .def(py::init<double, double, int>(), py::arg("base_price"),
           py::arg("tick_size"), py::arg("initial_capacity") = 1000)
      .def("process", &DenseVolProfile::process, "Add volume at price")
      .def("query", &DenseVolProfile::query, "Get total volume at price")
      .def("reset", &DenseVolProfile::reset, "Clear profile");

  // Sparse Volume Profile
  using SparseVolProfile = VolumeProfile<SparseStorage>;
  py::class_<SparseVolProfile>(m, "SparseVolumeProfile")
      .def(py::init<double>(), py::arg("tick_size"))
      .def("process", &SparseVolProfile::process, "Add volume at price")
      .def("query", &SparseVolProfile::query, "Get total volume at price")
      .def("reset", &SparseVolProfile::reset, "Clear profile");
}
