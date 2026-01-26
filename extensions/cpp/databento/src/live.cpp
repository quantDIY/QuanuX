#include <databento/live.hpp>
#include <databento/record.hpp>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace db = databento;

void bind_live(py::module &m) {
  // Live Builder
  py::class_<db::LiveBuilder>(m, "LiveBuilder")
      .def(py::init<>())
      .def("set_key_from_env", &db::LiveBuilder::SetKeyFromEnv)
      .def("set_key", &db::LiveBuilder::SetKey)
      // Use lambda for overload clarity
      .def("set_dataset",
           [](db::LiveBuilder &self, std::string dataset) {
             return self.SetDataset(dataset);
           })
      .def("build_blocking", &db::LiveBuilder::BuildBlocking);

  // Live Blocking Client
  py::class_<db::LiveBlocking>(m, "LiveBlocking")
      .def("subscribe",
           py::overload_cast<const std::vector<std::string> &, db::Schema,
                             db::SType>(&db::LiveBlocking::Subscribe),
           py::arg("symbols"), py::arg("schema"),
           py::arg("stype") = db::SType::RawSymbol)
      .def("subscribe_from",
           py::overload_cast<const std::vector<std::string> &, db::Schema,
                             db::SType, const std::string &>(
               &db::LiveBlocking::Subscribe),
           py::arg("symbols"), py::arg("schema"), py::arg("stype"),
           py::arg("start"))
      .def("start", &db::LiveBlocking::Start)
      .def("stop", &db::LiveBlocking::Stop)
      .def("__iter__", [](db::LiveBlocking &self) { return &self; })
      .def("__next__", [](db::LiveBlocking &self) -> py::object {
        // Block for next record, releasing GIL
        const db::Record *rec;
        {
          py::gil_scoped_release release;
          rec = self.NextRecord(
              std::chrono::seconds(1)); // 1s timeout to allow check for signals
        }
        if (!rec) {
          return py::none();
        }
        return py::cast(rec);
      });
}
