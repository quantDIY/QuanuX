#include <duckdb.hpp>
#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace duckdb;

// Wrapper classes to manage object lifetimes and simplify Python API
class PyDuckDB {
public:
  PyDuckDB(const std::string &path) : db(path) {}
  duckdb::DuckDB db;
};

class PyConnection {
public:
  PyConnection(PyDuckDB &py_db) : con(py_db.db) {}

  void query(const std::string &sql) {
    auto result = con.Query(sql);
    if (result->HasError()) {
      throw std::runtime_error(result->GetError());
    }
  }

  duckdb::Connection con;
};

class PyAppender {
public:
  PyAppender(PyConnection &py_con, const std::string &schema,
             const std::string &table)
      : appender(py_con.con, schema.empty() ? nullptr : schema.c_str(), table) {
  }

  PyAppender(PyConnection &py_con, const std::string &table)
      : appender(py_con.con, table) {}

  void begin_row() { appender.BeginRow(); }
  void end_row() { appender.EndRow(); }

  void append_bool(bool val) { appender.Append<bool>(val); }
  void append_int8(int8_t val) { appender.Append<int8_t>(val); }
  void append_int16(int16_t val) { appender.Append<int16_t>(val); }
  void append_int32(int32_t val) { appender.Append<int32_t>(val); }
  void append_int64(int64_t val) { appender.Append<int64_t>(val); }
  void append_uint8(uint8_t val) { appender.Append<uint8_t>(val); }
  void append_uint16(uint16_t val) { appender.Append<uint16_t>(val); }
  void append_uint32(uint32_t val) { appender.Append<uint32_t>(val); }
  void append_uint64(uint64_t val) { appender.Append<uint64_t>(val); }
  void append_float(float val) { appender.Append<float>(val); }
  void append_double(double val) { appender.Append<double>(val); }
  void append_string(const std::string &val) {
    appender.Append<const char *>(val.c_str());
  }

  void flush() { appender.Flush(); }
  void close() { appender.Close(); }

  // Expose the raw C++ pointer as a capsule for other C++ extensions to consume
  py::capsule get_capsule() {
    return py::capsule(&appender, "duckdb_appender");
  }

private:
  duckdb::Appender appender;
};

PYBIND11_MODULE(duckdb_py, m) {
  m.doc() = "DuckDB C++ Bindings for QuanuX";

  py::class_<PyDuckDB>(m, "DuckDB")
      .def(py::init<const std::string &>(), py::arg("path") = ":memory:");

  py::class_<PyConnection>(m, "Connection")
      .def(py::init<PyDuckDB &>())
      .def("query", &PyConnection::query, py::arg("sql"));

  py::class_<PyAppender>(m, "Appender")
      .def(py::init<PyConnection &, const std::string &, const std::string &>(),
           py::arg("connection"), py::arg("schema"), py::arg("table"))
      .def(py::init<PyConnection &, const std::string &>(),
           py::arg("connection"), py::arg("table"))
      .def("begin_row", &PyAppender::begin_row)
      .def("end_row", &PyAppender::end_row)
      .def("append_bool", &PyAppender::append_bool)
      .def("append_int64", &PyAppender::append_int64)
      .def("append_double", &PyAppender::append_double)
      .def("append_string", &PyAppender::append_string)
      .def("flush", &PyAppender::flush)
      .def("close", &PyAppender::close)
      .def("get_capsule", &PyAppender::get_capsule);
}
