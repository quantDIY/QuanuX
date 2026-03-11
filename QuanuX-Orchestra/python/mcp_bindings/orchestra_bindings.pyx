# Cython bindings for QuanuX Orchestra
__checksum__ = "a52a96d080c410e0f95298009ff33454a6ad52c2572c29638f7cfdb4aae69dab"

from orchestra_constants cimport FixTag

cpdef int get_tag_value(FixTag tag):
    return <int>tag
