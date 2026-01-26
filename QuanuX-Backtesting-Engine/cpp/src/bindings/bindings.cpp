#include "engine/metrics/PerformanceAnalyzer.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace quanux::engine::metrics;

PYBIND11_MODULE(quanux_metrics, m) {
  m.doc() = "QuanuX Backtesting Metrics Engine";

  py::class_<Trade>(m, "Trade")
      .def(py::init<>())
      .def_readwrite("entryPrice", &Trade::entryPrice)
      .def_readwrite("exitPrice", &Trade::exitPrice)
      .def_readwrite("profit", &Trade::profit)
      .def_readwrite("profitPct", &Trade::profitPct)
      .def_readwrite("risk", &Trade::risk)
      .def_readwrite("durationBars", &Trade::durationBars)
      .def_readwrite("isLong", &Trade::isLong)
      .def("__repr__", [](const Trade &t) {
        return "<quanux_metrics.Trade profit=" + std::to_string(t.profit) + ">";
      });

  py::class_<Metrics>(m, "Metrics")
      .def(py::init<>())
      .def_readonly("netProfit", &Metrics::netProfit)
      .def_readonly("grossProfit", &Metrics::grossProfit)
      .def_readonly("grossLoss", &Metrics::grossLoss)
      .def_readonly("profitFactor", &Metrics::profitFactor)
      .def_readonly("cagr", &Metrics::cagr)
      .def_readonly("winRate", &Metrics::winRate)
      .def_readonly("expectancy", &Metrics::expectancy)
      .def_readonly("maxDrawdownPct", &Metrics::maxDrawdownPct)
      .def_readonly("avgDrawdownPct", &Metrics::avgDrawdownPct)
      .def_readonly("ulcerIndex", &Metrics::ulcerIndex)
      .def_readonly("annualizedVol", &Metrics::annualizedVol)
      .def_readonly("sharpeRatio", &Metrics::sharpeRatio)
      .def_readonly("sortinoRatio", &Metrics::sortinoRatio)
      .def_readonly("calmarRatio", &Metrics::calmarRatio)
      .def_readonly("omegaRatio", &Metrics::omegaRatio)
      .def_readonly("informationRatio", &Metrics::informationRatio)
      .def_readonly("sqn", &Metrics::sqn)
      .def("__repr__", [](const Metrics &m) {
        return "<quanux_metrics.Metrics NetProfit=" +
               std::to_string(m.netProfit) +
               " Sharpe=" + std::to_string(m.sharpeRatio) + ">";
      });

  py::class_<PerformanceAnalyzer>(m, "PerformanceAnalyzer")
      .def(py::init<double, double>(), py::arg("startingEquity") = 10000.0,
           py::arg("riskFreeRate") = 0.0)
      .def("addTrade", &PerformanceAnalyzer::addTrade)
      .def("setEquityCurve", &PerformanceAnalyzer::setEquityCurve)
      .def("updateEquity", &PerformanceAnalyzer::updateEquity)
      .def("calculateMetrics", &PerformanceAnalyzer::calculateMetrics,
           py::arg("periodsPerYear") = 252);
}
