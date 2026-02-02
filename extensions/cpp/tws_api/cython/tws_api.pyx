# distutils: language = c++
# cython: language_level = 3

from libcpp.string cimport string
from libcpp.vector cimport vector

# ------------------------------------------------------------------------------
# External C++ Definitions
# ------------------------------------------------------------------------------

cdef extern from "QuanuX/Common.hpp" namespace "QuanuX":
    cdef enum Side:
        Buy "QuanuX::Side::Buy"
        Sell "QuanuX::Side::Sell"
        Short "QuanuX::Side::Short"
        Cover "QuanuX::Side::Cover"

    cdef struct Order:
        long id
        string symbol
        Side side
        double quantity
        double price
        string currency
        string exchange

cdef extern from "TwsAdapter.hpp" namespace "QuanuX::TwsApi":
    cdef cppclass TwsAdapter:
        TwsAdapter(string host, int port, int clientId)
        bint connect()
        void sendOrder(const Order& order)

# ------------------------------------------------------------------------------
# Python Classes
# ------------------------------------------------------------------------------

cdef class TwsAdapterWrapper:
    cdef TwsAdapter* _this

    def __cinit__(self, str host, int port, int clientId):
        self._this = new TwsAdapter(host.encode('utf-8'), port, clientId)
    
    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    def connect(self):
        """Connect to TWS or Gateway"""
        return self._this.connect()

    def send_order(self, int id, str symbol, str side, double qty, double price):
        """
        Send an order. 
        side: "BUY" or "SELL" (string) mapped to enum.
        """
        cdef Order order;
        order.id = id
        order.symbol = symbol.encode('utf-8')
        
        # Simple mapping logic matching original bind lambda
        if side == "BUY":
            order.side = Buy
        else:
            order.side = Sell
            
        order.quantity = qty
        order.price = price
        order.currency = b"USD"
        order.exchange = b"SMART"
        
        self._this.sendOrder(order)
