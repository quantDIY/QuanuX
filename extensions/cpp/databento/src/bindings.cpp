#include <databento/historical.hpp>
#include <databento/record.hpp>
#include <duckdb.hpp>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>

namespace py = pybind11;
namespace db = databento;

// Connector: Stream Databento data directly to DuckDB Appender
void timeseries_to_duckdb(db::Historical &client, const std::string &dataset,
                          const std::vector<std::string> &symbols,
                          const std::string &start, const std::string &end,
                          py::object appender_capsule) {

  duckdb::Appender *appender;
  if (py::isinstance<py::capsule>(appender_capsule)) {
    appender = appender_capsule.cast<py::capsule>();
  } else {
    throw std::runtime_error("Invalid appender capsule");
  }

  if (!appender)
    throw std::runtime_error("Appender is null");

  // Pre-declare callback to capture appender
  auto callback = [appender](const db::Record &record) {
    if (record.RType() == db::RType::Mbp0) { // Trades
      auto *trade_msg =
          reinterpret_cast<const db::TradeMsg *>(&record.Header());

      // Schema: ts_recv (BIGINT), price (BIGINT), size (UINT32), side (STRING),
      // action (STRING)
      appender->BeginRow();
      appender->Append<int64_t>(trade_msg->ts_recv.time_since_epoch().count());
      appender->Append<int64_t>(trade_msg->price);
      appender->Append<uint32_t>(trade_msg->size);
      char side_char = static_cast<char>(trade_msg->side);
      std::string side_str(1, side_char);
      appender->Append<const char *>(side_str.c_str());

      char action_char = static_cast<char>(trade_msg->action);
      std::string action_str(1, action_char);
      appender->Append<const char *>(action_str.c_str());
      appender->EndRow();
    } else if (record.RType() == db::RType::Mbo) {
      auto *mbo_msg = reinterpret_cast<const db::MboMsg *>(&record.Header());
      appender->BeginRow();
      appender->Append<int64_t>(mbo_msg->ts_recv.time_since_epoch().count());
      appender->Append<int64_t>(mbo_msg->price);
      appender->Append<uint32_t>(mbo_msg->size);
      // mbo_msg->side is char
      char side_char = static_cast<char>(mbo_msg->side);
      std::string side_str(1, side_char);
      appender->Append<const char *>(side_str.c_str());

      char action_char = static_cast<char>(mbo_msg->action);
      std::string action_str(1, action_char);
      appender->Append<const char *>(action_str.c_str());
      appender->EndRow();
    }
    return db::KeepGoing::Continue;
  };

  db::DateTimeRange<std::string> range{start, end};
  client.TimeseriesGetRange(dataset, range, symbols, db::Schema::Trades,
                            callback);
}

PYBIND11_MODULE(databento_py, m) {
  m.doc() = "Databento C++ API Python Wrapper";

  // Enums
  py::enum_<db::Schema>(m, "Schema")
      .value("Mbo", db::Schema::Mbo)
      .value("Mbp1", db::Schema::Mbp1)
      .value("Mbp10", db::Schema::Mbp10)
      .value("Tbbo", db::Schema::Tbbo)
      .value("Trades", db::Schema::Trades)
      .value("Ohlcv1S", db::Schema::Ohlcv1S)
      .value("Ohlcv1M", db::Schema::Ohlcv1M)
      .value("Ohlcv1H", db::Schema::Ohlcv1H)
      .value("Ohlcv1D", db::Schema::Ohlcv1D)
      .value("Definition", db::Schema::Definition)
      .value("Statistics", db::Schema::Statistics)
      .value("Status", db::Schema::Status)
      .export_values();

  py::enum_<db::SType>(m, "SType")
      .value("InstrumentId", db::SType::InstrumentId)
      .value("RawSymbol", db::SType::RawSymbol)
      .value("Parent", db::SType::Parent)
      .value("Continuous", db::SType::Continuous)
      .export_values();

  py::enum_<db::RType>(m, "RType")
      .value("Mbo", db::RType::Mbo)
      .value("Mbp1", db::RType::Mbp1)
      .value("Mbp0", db::RType::Mbp0)
      .value("Trades", db::RType::Mbp0)
      .value("Ohlcv1S", db::RType::Ohlcv1S)
      .export_values();

  // Data Structures
  py::class_<db::RecordHeader>(m, "RecordHeader")
      .def_readonly("instrument_id", &db::RecordHeader::instrument_id)
      .def_readonly("publisher_id", &db::RecordHeader::publisher_id)
      .def_readonly("ts_event", &db::RecordHeader::ts_event)
      .def_readonly("rtype", &db::RecordHeader::rtype);

  py::class_<db::MboMsg>(m, "MboMsg")
      .def_readonly("hd", &db::MboMsg::hd)
      .def_readonly("order_id", &db::MboMsg::order_id)
      .def_readonly("price", &db::MboMsg::price)
      .def_readonly("size", &db::MboMsg::size)
      // .def_readonly("flags", &db::MboMsg::flags)
      .def_readonly("action", &db::MboMsg::action)
      .def_readonly("side", &db::MboMsg::side)
      .def_readonly("ts_recv", &db::MboMsg::ts_recv);

  py::class_<db::TradeMsg>(m, "TradeMsg")
      .def_readonly("hd", &db::TradeMsg::hd)
      .def_readonly("price", &db::TradeMsg::price)
      .def_readonly("size", &db::TradeMsg::size)
      .def_readonly("side", &db::TradeMsg::side)
      .def_readonly("ts_recv", &db::TradeMsg::ts_recv);

  py::class_<db::Record>(m, "Record")
      .def("header", &db::Record::Header)
      .def("rtype", &db::Record::RType)
      .def("get_mbo",
           [](const db::Record &r) {
             if (r.RType() == db::RType::Mbo) {
               return *reinterpret_cast<const db::MboMsg *>(&r.Header());
             }
             throw std::runtime_error("Record does not contain MboMsg");
           })
      .def("get_trade", [](const db::Record &r) {
        if (r.RType() == db::RType::Mbp0) {
          return *reinterpret_cast<const db::TradeMsg *>(&r.Header());
        }
        throw std::runtime_error("Record does not contain TradeMsg (Mbp0)");
      });

  // Historical Builder
  py::class_<db::HistoricalBuilder>(m, "HistoricalBuilder")
      .def(py::init<>())
      .def("set_key_from_env", &db::HistoricalBuilder::SetKeyFromEnv)
      .def("set_key", &db::HistoricalBuilder::SetKey)
      .def("build", &db::HistoricalBuilder::Build);

  // Historical Client
  py::class_<db::Historical>(m, "Historical")
      .def(
          "timeseries_get_range",
          [](db::Historical &self, std::string dataset, std::string start,
             std::string end, std::vector<std::string> symbols,
             db::Schema schema,
             std::function<void(const db::Record &)> callback) {
            auto wrapped_cb = [&](const db::Record &rec) {
              // Acquire GIL before calling Python
              py::gil_scoped_acquire acquire;
              callback(rec);
              return db::KeepGoing::Continue;
            };
            // Release GIL before blocking
            py::gil_scoped_release release;
            self.TimeseriesGetRange(dataset, {start, end}, symbols, schema,
                                    wrapped_cb);
          },
          py::arg("dataset"), py::arg("start"), py::arg("end"),
          py::arg("symbols"), py::arg("schema"), py::arg("callback"))
      .def("timeseries_to_duckdb", &timeseries_to_duckdb, py::arg("dataset"),
           py::arg("symbols"), py::arg("start"), py::arg("end"),
           py::arg("appender_capsule"));
}
