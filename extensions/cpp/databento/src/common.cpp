#include <databento/record.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace db = databento;

void bind_common(py::module &m) {
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
}
