# cython: language_level=3
# cython: cplus=True

cdef extern from "quanux/MarketTick.hpp" namespace "quanux":
    cdef cppclass MarketTick:
        double price
        unsigned int instrument_id

cdef extern from "spreader/injected/CLZ4_CLF5_Spread_injected.hpp" namespace "quanux::spreader::strategy":
    cdef cppclass StrategyState:
        pass
    
    cdef cppclass InjectionStub:
        @staticmethod
        void init(StrategyState& state)
        
        @staticmethod
        bint calculate(StrategyState& state, const MarketTick& tick) nogil

cdef class PyStrategyWrapper:
    cdef StrategyState state

    def __cinit__(self):
        InjectionStub.init(self.state)

    def calculate(self, double price, unsigned int instrument_id):
        cdef MarketTick tick
        tick.price = price
        tick.instrument_id = instrument_id
        return InjectionStub.calculate(self.state, tick)
