# distutils: language = c++
# cython: language_level = 3

from libcpp.string cimport string
from libcpp.vector cimport vector
from libc.stdint cimport uint8_t, uint16_t, uint32_t, uint64_t, int64_t
from libcpp cimport bool
from libcpp.utility cimport move

# ------------------------------------------------------------------------------
# External C++ Definitions
# ------------------------------------------------------------------------------

cdef extern from "databento/enums.hpp" namespace "databento":
    cdef enum class Schema:
        Mbo, Mbp1, Mbp10, Tbbo, Trades, Ohlcv1S, Ohlcv1M, Ohlcv1H, Ohlcv1D, Definition, Statistics, Status

    cdef enum class SType:
        InstrumentId, RawSymbol, Parent, Continuous

    cdef enum class RType:
        Mbo, Mbp1, Mbp0, Ohlcv1S, Ohlcv1M, Ohlcv1H, Ohlcv1D, Definition, Statistics, Status

cdef extern from "databento/record.hpp" namespace "databento":
    cdef struct RecordHeader:
        uint32_t instrument_id
        uint16_t publisher_id
        uint64_t ts_event
        uint8_t rtype

    cdef struct MboMsg:
        RecordHeader hd
        uint64_t order_id
        int64_t price
        uint32_t size
        uint8_t flags
        char action
        char side
        uint64_t ts_recv
        uint64_t ts_in_delta
        uint32_t sequence

    cdef struct TradeMsg:
        RecordHeader hd
        int64_t price
        uint32_t size
        char action
        char side
        uint8_t flags
        uint8_t depth
        uint64_t ts_recv
        uint32_t sequence

    cdef cppclass Record:
        RecordHeader& Header()
        uint8_t RType()

cdef extern from "databento/historical.hpp" namespace "databento":
    cdef cppclass Historical:
        Historical() except +
        Historical(Historical& other) except +
        # TimeseriesGetRange is complex with callback. 
        # For now, we wrap the builder or just the blocking client calls if available?
        # The C++ Client has TimeseriesGetRange taking a callback.
        
    cdef cppclass HistoricalBuilder:
        HistoricalBuilder() except +
        HistoricalBuilder& SetKeyFromEnv()
        HistoricalBuilder& SetKey(string key)
        Historical Build()

# ------------------------------------------------------------------------------
# Python Classes
# ------------------------------------------------------------------------------

cdef class PyHistoricalBuilder:
    cdef HistoricalBuilder* _this

    def __cinit__(self):
        self._this = new HistoricalBuilder()

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    def set_key_from_env(self):
        self._this.SetKeyFromEnv()
        return self

    def set_key(self, str key):
        self._this.SetKey(key.encode('utf-8'))
        return self

    def build(self):
        # Returns a Historical client.
        return PyHistorical.create(move(self._this.Build()))

cdef class PyHistorical:
    cdef Historical* _this

    def __cinit__(self):
         self._this = NULL # initialized via factory

    def __dealloc__(self):
        if self._this != NULL:
            del self._this
    
    @staticmethod
    cdef create(Historical client):
        cdef PyHistorical wrapper = PyHistorical()
        wrapper._this = new Historical(move(client)) # Move wrapper
        return wrapper

    # TODO: Implement timeseries_get_range wrapper
    # This requires callback Shim.
