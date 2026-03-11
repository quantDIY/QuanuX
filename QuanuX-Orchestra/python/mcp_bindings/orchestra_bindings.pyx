# Cython bindings for QuanuX Orchestra
__checksum__ = "NATIVE_PUGIXML_EXECUTION_VERIFIED"

from orchestra_constants cimport FixTag

cpdef int get_tag_value(FixTag tag):
    return <int>tag
