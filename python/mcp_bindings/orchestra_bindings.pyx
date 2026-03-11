# Cython bindings for QuanuX Orchestra
__checksum__ = "demo_sha256_checksum"

from orchestra_constants cimport FixTag

cpdef int get_tag_value(FixTag tag):
    return <int>tag
