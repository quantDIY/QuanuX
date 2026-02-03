# cython: language_level = 3

cdef class Adapter:
    cdef public object name
    cdef public object config
    
    # We declare methods we want to access from Cython via vtable if needed,
    # but since these are async python methods we invoke them as python objects usually.
    # However, to use "Adapter" as a type hint in Core, this PXD is required.
