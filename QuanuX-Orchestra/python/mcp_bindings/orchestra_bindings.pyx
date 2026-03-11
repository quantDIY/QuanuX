# Cython bindings for QuanuX Orchestra
__checksum__ = "9e890164a0bb554211ba352cdbbec9457d1e1ff194b7781fb1674719442ce84d"

from orchestra_constants cimport FixTag

cpdef int get_tag_value(FixTag tag):
    return <int>tag
