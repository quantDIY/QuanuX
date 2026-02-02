# distutils: language = c++
# cython: language_level = 3

from libcpp.vector cimport vector
from libcpp.memory cimport unique_ptr
from libcpp.string cimport string
from libc.stdint cimport int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t
from libcpp cimport bool

ctypedef const char* const_char_ptr

# ------------------------------------------------------------------------------
# External C++ Definitions
# ------------------------------------------------------------------------------

cdef extern from "../third_party/duckdb.hpp" namespace "duckdb":
    cdef cppclass DuckDB:
        DuckDB(const string& path) except +

    cdef cppclass MaterializedQueryResult:
        bool HasError()
        string GetError()

    cdef cppclass Connection:
        Connection(DuckDB& db) except +
        unique_ptr[MaterializedQueryResult] Query(string query)
        
    cdef cppclass Appender:
        Appender(Connection& con, string schema, string table) except +
        Appender(Connection& con, string table) except +
        
        void BeginRow()
        void EndRow()
        
        # Template method mapping
        void Append[T](T val)
        
        void Flush()
        void Close()

# ------------------------------------------------------------------------------
# Python Classes
# ------------------------------------------------------------------------------

cdef class PyDuckDB:
    cdef DuckDB* _this

    def __cinit__(self, str path=":memory:"):
        self._this = new DuckDB(path.encode('utf-8'))

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

cdef class PyConnection:
    cdef Connection* _this
    cdef PyDuckDB _db_ref # Keep db alive

    def __cinit__(self, PyDuckDB db):
        self._db_ref = db
        self._this = new Connection(db._this[0])

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    def query(self, str sql):
        cdef unique_ptr[MaterializedQueryResult] res = self._this.Query(sql.encode('utf-8'))
        if res.get().HasError():
            # Dereference unique_ptr to access object
            raise RuntimeError(res.get().GetError().decode('utf-8'))

cdef class PyAppender:
    cdef Appender* _this
    cdef PyConnection _con_ref # Keep connection alive

    def __cinit__(self, PyConnection con, str table, str schema=""):
        self._con_ref = con
        if schema:
            self._this = new Appender(con._this[0], schema.encode('utf-8'), table.encode('utf-8'))
        else:
            self._this = new Appender(con._this[0], table.encode('utf-8'))

    def __dealloc__(self):
        if self._this != NULL:
            del self._this

    def begin_row(self):
        self._this.BeginRow()

    def end_row(self):
        self._this.EndRow()

    def flush(self):
        self._this.Flush()

    def close(self):
        self._this.Close()

    # Append wrappers
    def append_bool(self, bool val): self._this.Append[bool](val)
    def append_int8(self, int8_t val): self._this.Append[int8_t](val)
    def append_int16(self, int16_t val): self._this.Append[int16_t](val)
    def append_int32(self, int32_t val): self._this.Append[int32_t](val)
    def append_int64(self, int64_t val): self._this.Append[int64_t](val)
    def append_uint8(self, uint8_t val): self._this.Append[uint8_t](val)
    def append_uint16(self, uint16_t val): self._this.Append[uint16_t](val)
    def append_uint32(self, uint32_t val): self._this.Append[uint32_t](val)
    def append_uint64(self, uint64_t val): self._this.Append[uint64_t](val)
    def append_float(self, float val): self._this.Append[float](val)
    def append_double(self, double val): self._this.Append[double](val)
    
    def append_string(self, str val): 
        # DuckDB generic Append works for strings/char* too?
        # duckdb.hpp: Append(const char* val)
        self._this.Append[const_char_ptr](val.encode('utf-8'))

