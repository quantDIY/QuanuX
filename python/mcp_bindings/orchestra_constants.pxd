# Cython declarations for QuanuX Orchestra
cdef extern from "../../include/quanux/orchestra/constants.hpp" namespace "quanux::orchestra":
    cpdef enum class FixTag(unsigned int):
        quanux_unmapped = 99999
