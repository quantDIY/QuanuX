#ifndef CALLBACK_SHIM_HPP
#define CALLBACK_SHIM_HPP

#include "RApiPlus.h"
#include <Python.h>
#include <iostream>
#include <string>

class CallbackShim : public RApi::RCallbacks {
public:
  PyObject *_py_obj; // Reference to the python object (weak or strong? usually
                     // strong if we own it, or weak if cyclic)

  CallbackShim(PyObject *obj) : _py_obj(obj) { Py_XINCREF(_py_obj); }

  virtual ~CallbackShim() { Py_XDECREF(_py_obj); }

  // Helper to call python method with a capsule
  int CallPy(const char *method, const char *name, void *ptr, void *pContext,
             int *aiCode) {
    PyGILState_STATE gstate = PyGILState_Ensure();

    // Wrap ptr
    PyObject *cap = PyCapsule_New(ptr, name, NULL);
    if (!cap) {
      PyGILState_Release(gstate);
      return 0;
    }

    // Wrap Context (if needed? Context is void*, usually ignored in Py wrappers
    // or passed as capsule too) For simplicity, we ignore Context for now or
    // pass as Null

    PyObject *result = PyObject_CallMethod(_py_obj, method, "O", cap);

    int ret = 0;
    if (result) {
      // Parse result if needed, usually callbacks return int code
      if (PyLong_Check(result)) {
        ret = (int)PyLong_AsLong(result);
      }
      Py_DECREF(result);
    } else {
      PyErr_Print(); // Print error to stderr
    }

    Py_DECREF(cap);
    PyGILState_Release(gstate);
    return ret;
  }

  // --- Implement Virtual Methods ---

  virtual int Alert(RApi::AlertInfo *pInfo, void *pContext,
                    int *aiCode) override {
    return CallPy("_on_alert", "AlertInfo", (void *)pInfo, pContext, aiCode);
  }

  virtual int LineUpdate(RApi::LineInfo *pInfo, void *pContext,
                         int *aiCode) override {
    return CallPy("_on_line_update", "LineInfo", (void *)pInfo, pContext,
                  aiCode);
  }

  virtual int FillReport(RApi::OrderFillReport *pReport, void *pContext,
                         int *aiCode) override {
    return CallPy("_on_fill_report", "OrderFillReport", (void *)pReport,
                  pContext, aiCode);
  }

  virtual int FailureReport(RApi::OrderFailureReport *pReport, void *pContext,
                            int *aiCode) override {
    return CallPy("_on_failure_report", "OrderFailureReport", (void *)pReport,
                  pContext, aiCode);
  }

  virtual int BestBidQuote(RApi::BidInfo *pInfo, void *pContext,
                           int *aiCode) override {
    return CallPy("_on_best_bid_quote", "BidInfo", (void *)pInfo, pContext,
                  aiCode);
  }

  virtual int BestAskQuote(RApi::AskInfo *pInfo, void *pContext,
                           int *aiCode) override {
    return CallPy("_on_best_ask_quote", "AskInfo", (void *)pInfo, pContext,
                  aiCode);
  }

  virtual int TradePrint(RApi::TradeInfo *pInfo, void *pContext,
                         int *aiCode) override {
    return CallPy("_on_trade_print", "TradeInfo", (void *)pInfo, pContext,
                  aiCode);
  }

  // TODO: Add other callbacks if needed (e.g. AccountInfo, etc.)
  // But these are the ones from bindings.cpp
};

#endif
