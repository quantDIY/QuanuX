# distutils: language = c++
# cython: language_level = 3

from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp.pair cimport pair
from libcpp.utility cimport move
import cython

# ------------------------------------------------------------------------------
# C++ Extern Definitions
# ------------------------------------------------------------------------------

cdef extern from "quanux/indicators/sma.hpp" namespace "quanux::indicators":
    cdef vector[double] compute_sma[T](T data, int period)

cdef extern from "quanux/indicators/market_profile.hpp" namespace "quanux::indicators":
    cdef cppclass DenseStorage:
        DenseStorage(double, double, int)
    
    cdef cppclass SparseStorage:
        SparseStorage(double)

    cdef cppclass MarketProfile[Policy]:
        MarketProfile(Policy) # This constructor might be tricky if it expects args forwarded
        # Actually existing bindings wrap concrete instantiations. 
        # C++: MarketProfile(Args&&...)
        # We can map specific constructors for specific template instantiations if needed
        # or just instantiate the helper classes in cython?
        
        void process(double price)
        int query(double price)
        void reset()

cdef extern from "quanux/indicators/volume_profile.hpp" namespace "quanux::indicators":
    cdef cppclass VolumeProfile[Policy]:
        void process(double price, double volume)
        double query(double price)
        void reset()

# We need to explicitly define the concrete template specializations we want to use
# because Cython needs to know the exact C++ type to emit 'new MarketProfile<DenseStorage>(...)'

cdef extern from "quanux/indicators/market_profile.hpp" namespace "quanux::indicators":
    cdef cppclass DenseProfile "quanux::indicators::MarketProfile<quanux::indicators::DenseStorage>":
        DenseProfile(double base_price, double tick_size, int initial_capacity)
        void process(double price)
        int query(double price)
        void reset()

    cdef cppclass SparseProfile "quanux::indicators::MarketProfile<quanux::indicators::SparseStorage>":
        SparseProfile(double tick_size)
        void process(double price)
        int query(double price)
        void reset()

cdef extern from "quanux/indicators/volume_profile.hpp" namespace "quanux::indicators":
    cdef cppclass DenseVolProfile "quanux::indicators::VolumeProfile<quanux::indicators::DenseStorage>":
        DenseVolProfile(double base_price, double tick_size, int initial_capacity)
        void process(double price, double volume)
        double query(double price)
        void reset()

    cdef cppclass SparseVolProfile "quanux::indicators::VolumeProfile<quanux::indicators::SparseStorage>":
        SparseVolProfile(double tick_size)
        void process(double price, double volume)
        double query(double price)
        void reset()


# ------------------------------------------------------------------------------
# Python Classes
# ------------------------------------------------------------------------------

# 1. SMA (Stateful Python Implementation for O(1) updates)
# The C++ `sma_view` is for static ranges, not online updates. 
# Implementing a fast RingBuffer SMA in Cython is better for `test_indicators.py`
cdef class SMA:
    cdef int period
    cdef double sum
    cdef vector[double] ring_buffer
    cdef int head
    cdef int count

    def __init__(self, int period):
        self.period = period
        self.sum = 0.0
        self.head = 0
        self.count = 0
        self.ring_buffer.resize(period, 0.0)

    cpdef double update(self, double value):
        cdef double removed = 0.0
        if self.count < self.period:
            self.count += 1
        else:
            removed = self.ring_buffer[self.head]
            self.sum -= removed
        
        self.ring_buffer[self.head] = value
        self.sum += value
        
        self.head = (self.head + 1) % self.period
        
        return self.sum / self.count

    cpdef double value(self):
        if self.count == 0:
            return 0.0
        return self.sum / self.count

# 2. Market Profile Wrappers

cdef class DenseMarketProfile:
    cdef DenseProfile* _this

    def __cinit__(self, double base_price, double tick_size, int initial_capacity=1000):
        self._this = new DenseProfile(base_price, tick_size, initial_capacity)

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    cpdef void process(self, double price):
        self._this.process(price)

    cpdef int query(self, double price):
        return self._this.query(price)

    cpdef void reset(self):
        self._this.reset()

cdef class SparseMarketProfile:
    cdef SparseProfile* _this

    def __cinit__(self, double tick_size):
        self._this = new SparseProfile(tick_size)

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    cpdef void process(self, double price):
        self._this.process(price)

    cpdef int query(self, double price):
        return self._this.query(price)

    cpdef void reset(self):
        self._this.reset()

# 3. Volume Profile Wrappers

cdef class DenseVolumeProfile:
    cdef DenseVolProfile* _this

    def __cinit__(self, double base_price, double tick_size, int initial_capacity=1000):
        self._this = new DenseVolProfile(base_price, tick_size, initial_capacity)

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    cpdef void process(self, double price, double volume):
        self._this.process(price, volume)

    cpdef double query(self, double price):
        return self._this.query(price)

    cpdef void reset(self):
        self._this.reset()

cdef class SparseVolumeProfile:
    cdef SparseVolProfile* _this

    def __cinit__(self, double tick_size):
        self._this = new SparseVolProfile(tick_size)

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    cpdef void process(self, double price, double volume):
        self._this.process(price, volume)

    cpdef double query(self, double price):
        return self._this.query(price)

    cpdef void reset(self):
        self._this.reset()

# 4. Batch Helpers via C++ (e.g. compute_sma)
def compute_sma_batch(vector[double] data, int period):
    return compute_sma(move(data), period)
