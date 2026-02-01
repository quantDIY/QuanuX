#include "../shim/TwsAdapter.hpp"

#if __has_include(<pybind11/pybind11.h>)
#include <pybind11/pybind11.h>
namespace py = pybind11;
#else
// Mock pybind11 for linter/IntelliSense when header is missing in dev
// environment
#include <string>
namespace pybind11 {
struct arg {
  arg(const char *) {}
};
template <typename... Args> struct init {};

struct module_ {
  const char *_doc;
  const char *&doc() { return _doc; }
};

template <typename T> struct class_ {
  class_(module_ &, const char *) {}
  class_ &def(init<const std::string &, int, int>, arg, arg, arg) {
    return *this;
  }
  template <typename Func> class_ &def(const char *, Func, const char * = "") {
    return *this;
  }
};
} // namespace pybind11
namespace py = pybind11;
#define PYBIND11_MODULE(name, variable)                                        \
  void name##_module_init(py::module_ &variable)
#endif

PYBIND11_MODULE(TwsApi, m) {
  m.doc() = "QuanuX TWS API Adapter wrapping Native C++ SDK";

  py::class_<QuanuX::TwsApi::TwsAdapter>(m, "TwsAdapter")
      .def(py::init<const std::string &, int, int>(), py::arg("host"),
           py::arg("port"), py::arg("clientId"))
      .def("connect", &QuanuX::TwsApi::TwsAdapter::connect,
           "Connect to TWS or Gateway")
      .def(
          "send_order",
          [](QuanuX::TwsApi::TwsAdapter &self, int id,
             const std::string &symbol, const std::string &side, double qty,
             double price) {
            // Simplified pythonic wrapper for the internal sendOrder
            // In a real impl, we'd map this perfectly to the QuanuX Order
            // struct For now, this proves the binding works.
            QuanuX::Order order;
            order.id = id;
            order.symbol = symbol;
            order.side =
                (side == "BUY" ? QuanuX::Side::Buy : QuanuX::Side::Sell);
            order.quantity = qty;
            order.price = price;
            self.sendOrder(order);
          },
          "Send an order via the generic interface");
}
