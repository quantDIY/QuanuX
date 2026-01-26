#include "RApiPlus.h"
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

// --- Helper Functions for tsNCharcb ---

std::string from_cb(const tsNCharcb &cb) {
  if (!cb.pData)
    return "";
  return std::string(cb.pData, cb.iDataLen);
}

void to_cb(tsNCharcb &cb, const std::string &s) {
  cb.pData = const_cast<char *>(s.c_str());
  cb.iDataLen = static_cast<int>(s.length());
}

// --- RCallbacks Trampoline ---
class PyRCallbacks : public RApi::RCallbacks {
public:
  using RApi::RCallbacks::RCallbacks;

  int Alert(RApi::AlertInfo *pInfo, void *pContext, int *aiCode) override {
    py::gil_scoped_acquire acquire;
    PYBIND11_OVERRIDE(int, RApi::RCallbacks, Alert, pInfo, pContext, aiCode);
  }

  int LineUpdate(RApi::LineInfo *pInfo, void *pContext, int *aiCode) override {
    py::gil_scoped_acquire acquire;
    PYBIND11_OVERRIDE(int, RApi::RCallbacks, LineUpdate, pInfo, pContext,
                      aiCode);
  }

  int FillReport(RApi::OrderFillReport *pReport, void *pContext,
                 int *aiCode) override {
    py::gil_scoped_acquire acquire;
    PYBIND11_OVERRIDE(int, RApi::RCallbacks, FillReport, pReport, pContext,
                      aiCode);
  }

  int FailureReport(RApi::OrderFailureReport *pReport, void *pContext,
                    int *aiCode) override {
    py::gil_scoped_acquire acquire;
    PYBIND11_OVERRIDE(int, RApi::RCallbacks, FailureReport, pReport, pContext,
                      aiCode);
  }

  // Market Data Callbacks
  int BestBidQuote(RApi::BidInfo *pInfo, void *pContext, int *aiCode) override {
    py::gil_scoped_acquire acquire;
    PYBIND11_OVERRIDE(int, RApi::RCallbacks, BestBidQuote, pInfo, pContext,
                      aiCode);
  }

  int BestAskQuote(RApi::AskInfo *pInfo, void *pContext, int *aiCode) override {
    py::gil_scoped_acquire acquire;
    PYBIND11_OVERRIDE(int, RApi::RCallbacks, BestAskQuote, pInfo, pContext,
                      aiCode);
  }

  // Trade Data
  int TradePrint(RApi::TradeInfo *pInfo, void *pContext, int *aiCode) override {
    py::gil_scoped_acquire acquire;
    PYBIND11_OVERRIDE(int, RApi::RCallbacks, TradePrint, pInfo, pContext,
                      aiCode);
  }
};

PYBIND11_MODULE(rithmic_py, m) {
  m.doc() = "Rithmic R | API+ Python Wrapper";

  // --- Data Structures ---

  py::class_<RApi::REngineParams>(m, "REngineParams")
      .def(py::init<>())
      .def_property(
          "sAppName",
          [](const RApi::REngineParams &p) { return from_cb(p.sAppName); },
          [](RApi::REngineParams &p, const std::string &s) {
            to_cb(p.sAppName, s);
          })
      .def_property(
          "sAppVersion",
          [](const RApi::REngineParams &p) { return from_cb(p.sAppVersion); },
          [](RApi::REngineParams &p, const std::string &s) {
            to_cb(p.sAppVersion, s);
          })
      .def_property(
          "sLogFilePath",
          [](const RApi::REngineParams &p) { return from_cb(p.sLogFilePath); },
          [](RApi::REngineParams &p, const std::string &s) {
            to_cb(p.sLogFilePath, s);
          });

  py::class_<RApi::LoginParams>(m, "LoginParams")
      .def(py::init<>())
      .def_property(
          "sMdUser",
          [](const RApi::LoginParams &p) { return from_cb(p.sMdUser); },
          [](RApi::LoginParams &p, const std::string &s) {
            to_cb(p.sMdUser, s);
          })
      .def_property(
          "sMdPassword",
          [](const RApi::LoginParams &p) { return from_cb(p.sMdPassword); },
          [](RApi::LoginParams &p, const std::string &s) {
            to_cb(p.sMdPassword, s);
          })
      .def_property(
          "sMdCnnctPt",
          [](const RApi::LoginParams &p) { return from_cb(p.sMdCnnctPt); },
          [](RApi::LoginParams &p, const std::string &s) {
            to_cb(p.sMdCnnctPt, s);
          })
      .def_property(
          "sTsUser",
          [](const RApi::LoginParams &p) { return from_cb(p.sTsUser); },
          [](RApi::LoginParams &p, const std::string &s) {
            to_cb(p.sTsUser, s);
          })
      .def_property(
          "sTsPassword",
          [](const RApi::LoginParams &p) { return from_cb(p.sTsPassword); },
          [](RApi::LoginParams &p, const std::string &s) {
            to_cb(p.sTsPassword, s);
          })
      .def_property(
          "sTsCnnctPt",
          [](const RApi::LoginParams &p) { return from_cb(p.sTsCnnctPt); },
          [](RApi::LoginParams &p, const std::string &s) {
            to_cb(p.sTsCnnctPt, s);
          })
      .def_readwrite("pCallbacks", &RApi::LoginParams::pCallbacks);

  py::class_<RApi::AlertInfo>(m, "AlertInfo")
      .def(py::init<>())
      .def_readonly("iAlertType", &RApi::AlertInfo::iAlertType)
      .def_readonly("iConnectionId", &RApi::AlertInfo::iConnectionId)
      .def_readonly("iRpCode", &RApi::AlertInfo::iRpCode)
      .def_property_readonly(
          "sRpCode",
          [](const RApi::AlertInfo &p) { return from_cb(p.sRpCode); })
      .def_property_readonly(
          "sMessage",
          [](const RApi::AlertInfo &p) { return from_cb(p.sMessage); })
      .def_property_readonly(
          "sTicker",
          [](const RApi::AlertInfo &p) { return from_cb(p.sTicker); })
      .def_property_readonly("sExchange", [](const RApi::AlertInfo &p) {
        return from_cb(p.sExchange);
      });

  py::class_<RApi::LineInfo>(m, "LineInfo")
      .def(py::init<>())
      .def_property_readonly(
          "sTicker", [](const RApi::LineInfo &p) { return from_cb(p.sTicker); })
      .def_property_readonly(
          "sExchange",
          [](const RApi::LineInfo &p) { return from_cb(p.sExchange); })
      .def_property_readonly(
          "sOrderNum",
          [](const RApi::LineInfo &p) { return from_cb(p.sOrderNum); })
      .def_readonly("llQuantityToFill", &RApi::LineInfo::llQuantityToFill)
      .def_readonly("dPriceToFill", &RApi::LineInfo::dPriceToFill);

  // Reports
  py::class_<RApi::OrderFillReport>(m, "OrderFillReport")
      .def(py::init<>())
      .def_property_readonly(
          "sTicker",
          [](const RApi::OrderFillReport &p) { return from_cb(p.sTicker); })
      .def_property_readonly(
          "sExchange",
          [](const RApi::OrderFillReport &p) { return from_cb(p.sExchange); })
      .def_property_readonly(
          "sOrderNum",
          [](const RApi::OrderFillReport &p) { return from_cb(p.sOrderNum); })
      .def_readonly("dFillPrice", &RApi::OrderFillReport::dFillPrice)
      .def_readonly("llFillSize", &RApi::OrderFillReport::llFillSize)
      .def_property_readonly("sFillType", [](const RApi::OrderFillReport &p) {
        return from_cb(p.sFillType);
      });

  py::class_<RApi::OrderFailureReport>(m, "OrderFailureReport")
      .def(py::init<>())
      .def_property_readonly(
          "sTicker",
          [](const RApi::OrderFailureReport &p) { return from_cb(p.sTicker); })
      .def_property_readonly("sExchange",
                             [](const RApi::OrderFailureReport &p) {
                               return from_cb(p.sExchange);
                             })
      .def_property_readonly(
          "sStatus",
          [](const RApi::OrderFailureReport &p) { return from_cb(p.sStatus); })
      .def_property_readonly("sOrderNum",
                             [](const RApi::OrderFailureReport &p) {
                               return from_cb(p.sOrderNum);
                             });

  // Market Data Info
  py::class_<RApi::BidInfo>(m, "BidInfo")
      .def(py::init<>())
      .def_property_readonly(
          "sTicker", [](const RApi::BidInfo &p) { return from_cb(p.sTicker); })
      .def_property_readonly(
          "sExchange",
          [](const RApi::BidInfo &p) { return from_cb(p.sExchange); })
      .def_readonly("dPrice", &RApi::BidInfo::dPrice)
      .def_readonly("llSize", &RApi::BidInfo::llSize);

  py::class_<RApi::AskInfo>(m, "AskInfo")
      .def(py::init<>())
      .def_property_readonly(
          "sTicker", [](const RApi::AskInfo &p) { return from_cb(p.sTicker); })
      .def_property_readonly(
          "sExchange",
          [](const RApi::AskInfo &p) { return from_cb(p.sExchange); })
      .def_readonly("dPrice", &RApi::AskInfo::dPrice)
      .def_readonly("llSize", &RApi::AskInfo::llSize);

  py::class_<RApi::TradeInfo>(m, "TradeInfo")
      .def(py::init<>())
      .def_property_readonly(
          "sTicker",
          [](const RApi::TradeInfo &p) { return from_cb(p.sTicker); })
      .def_property_readonly(
          "sExchange",
          [](const RApi::TradeInfo &p) { return from_cb(p.sExchange); })
      .def_readonly("dPrice", &RApi::TradeInfo::dPrice)
      .def_readonly("llSize", &RApi::TradeInfo::llSize)
      .def_property_readonly("sAggressorSide", [](const RApi::TradeInfo &p) {
        return from_cb(p.sAggressorSide);
      });

  // Order Params
  py::class_<RApi::LimitOrderParams>(m, "LimitOrderParams")
      .def(py::init<>())
      .def_property(
          "sTicker",
          [](const RApi::LimitOrderParams &p) { return from_cb(p.sTicker); },
          [](RApi::LimitOrderParams &p, const std::string &s) {
            to_cb(p.sTicker, s);
          })
      .def_property(
          "sExchange",
          [](const RApi::LimitOrderParams &p) { return from_cb(p.sExchange); },
          [](RApi::LimitOrderParams &p, const std::string &s) {
            to_cb(p.sExchange, s);
          })
      .def_property(
          "sBuySellType",
          [](const RApi::LimitOrderParams &p) {
            return from_cb(p.sBuySellType);
          },
          [](RApi::LimitOrderParams &p, const std::string &s) {
            to_cb(p.sBuySellType, s);
          })
      .def_readwrite("dPrice", &RApi::LimitOrderParams::dPrice)
      .def_readwrite("iQty", &RApi::LimitOrderParams::iQty)
      .def_property(
          "sDuration",
          [](const RApi::LimitOrderParams &p) { return from_cb(p.sDuration); },
          [](RApi::LimitOrderParams &p, const std::string &s) {
            to_cb(p.sDuration, s);
          });

  // Modify Order Params
  py::class_<RApi::ModifyLimitOrderParams>(m, "ModifyLimitOrderParams")
      .def(py::init<>())
      .def_property(
          "sTicker",
          [](const RApi::ModifyLimitOrderParams &p) {
            return from_cb(p.sTicker);
          },
          [](RApi::ModifyLimitOrderParams &p, const std::string &s) {
            to_cb(p.sTicker, s);
          })
      .def_property(
          "sExchange",
          [](const RApi::ModifyLimitOrderParams &p) {
            return from_cb(p.sExchange);
          },
          [](RApi::ModifyLimitOrderParams &p, const std::string &s) {
            to_cb(p.sExchange, s);
          })
      .def_property(
          "sOrderNum",
          [](const RApi::ModifyLimitOrderParams &p) {
            return from_cb(p.sOrderNum);
          },
          [](RApi::ModifyLimitOrderParams &p, const std::string &s) {
            to_cb(p.sOrderNum, s);
          })
      .def_readwrite("dPrice", &RApi::ModifyLimitOrderParams::dPrice)
      .def_readwrite("iQty", &RApi::ModifyLimitOrderParams::iQty);

  // Order Params (needed for Cancel? Actually usually cancelOrder takes
  // OrderParams or just OrderNum etc. RApiPlus.h: int cancelOrder(OrderParams *
  // pParams, int * aiCode); matches logic? Wait, cancelOrder not in REngine
  // methods listed above? Checking RApiPlus.h again locally? Usually Cancel is
  // via modify with 0 qty or specific cancel methods. Ah, sendOrder(Calculated)
  // or similar? Let's check REngine methods... It has sendOrder. Does it have
  // cancelOrder? Previous grep didn't show it but we can assume it might exist
  // or we use modify. Actually standard RApi has `cancelOrder(OrderParams *
  // ...)`? Let's bind what we see or what is standard. If not found in previous
  // grep, maybe it is `sendOrderList` or `modifyOrder`... Actually, Rithmic
  // usually has `sendOrder` with Valid cancel params. But most APIs have
  // `cancelOrder`. Let's assume we use regular `OrderParams` for cancel.

  // RCallback bindings
  py::class_<RApi::RCallbacks, PyRCallbacks>(m, "RCallbacks")
      .def(py::init<>())
      .def("Alert", &RApi::RCallbacks::Alert)
      .def("LineUpdate", &RApi::RCallbacks::LineUpdate)
      .def("FillReport", &RApi::RCallbacks::FillReport)
      .def("FailureReport", &RApi::RCallbacks::FailureReport)
      .def("BestBidQuote", &RApi::RCallbacks::BestBidQuote)
      .def("BestAskQuote", &RApi::RCallbacks::BestAskQuote)
      .def("TradePrint", &RApi::RCallbacks::TradePrint);

  // --- REngine ---
  py::class_<RApi::REngine>(m, "REngine")
      .def(py::init([](RApi::REngineParams *p) {
        try {
          return new RApi::REngine(p);
        } catch (OmneException &e) {
          throw std::runtime_error(std::string("REngine Init Failed: ") +
                                   e.getErrorString());
        } catch (const std::exception &e) {
          throw std::runtime_error(std::string("REngine Init StdException: ") +
                                   e.what());
        } catch (...) {
          throw std::runtime_error("REngine Init Failed: Unknown Exception");
        }
      }))
      .def("login",
           [](RApi::REngine &self, RApi::LoginParams *params) {
             py::gil_scoped_release release;
             int iCode = 0;
             return self.login(params, &iCode);
           })
      .def("logout",
           [](RApi::REngine &self) {
             py::gil_scoped_release release;
             int iCode = 0;
             return self.logout(&iCode);
           })
      .def(
          "subscribe",
          [](RApi::REngine &self, std::string exchange, std::string ticker,
             int flags) {
            tsNCharcb sExch = {const_cast<char *>(exchange.c_str()),
                               (int)exchange.length()};
            tsNCharcb sTick = {const_cast<char *>(ticker.c_str()),
                               (int)ticker.length()};
            int iCode = 0;
            return self.subscribe(&sExch, &sTick, flags, &iCode);
          },
          py::arg("exchange"), py::arg("ticker"), py::arg("flags"))
      .def(
          "unsubscribe",
          [](RApi::REngine &self, std::string exchange, std::string ticker) {
            tsNCharcb sExch = {const_cast<char *>(exchange.c_str()),
                               (int)exchange.length()};
            tsNCharcb sTick = {const_cast<char *>(ticker.c_str()),
                               (int)ticker.length()};
            int iCode = 0;
            return self.unsubscribe(&sExch, &sTick, &iCode);
          },
          py::arg("exchange"), py::arg("ticker"))
      .def("sendOrder",
           [](RApi::REngine &self, RApi::LimitOrderParams *params) {
             int iCode = 0;
             return self.sendOrder(params, &iCode);
           })
      .def("modifyOrder",
           [](RApi::REngine &self, RApi::ModifyLimitOrderParams *params) {
             int iCode = 0;
             return self.modifyOrder(params, &iCode);
           });

  m.def("version", []() { return "0.1.3-wrapper"; }, "Get wrapper version");
}
