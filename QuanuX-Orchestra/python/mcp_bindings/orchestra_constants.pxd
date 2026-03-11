# Cython declarations for QuanuX Orchestra
cdef extern from "../../include/quanux/orchestra/constants.hpp" namespace "quanux::orchestra":
    cpdef enum class FixTag(unsigned int):
        BeginString = 8
        BodyLength = 9
        ClOrdID = 11
        MsgType = 35
        OrderQty = 38
        Price = 44
        Side = 54
        Symbol = 55
        TransactTime = 60
        quanux_unmapped = 99999
