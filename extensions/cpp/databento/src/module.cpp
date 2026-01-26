#include <pybind11/pybind11.h>

namespace py = pybind11;

// Forward declarations
void bind_common(py::module &m);
void bind_historical(py::module &m);
void bind_live(py::module &m);

PYBIND11_MODULE(databento_py, m) {
  m.doc() = "Databento C++ API Python Wrapper";

  bind_common(m);
  bind_historical(m);
  bind_live(m);
}
