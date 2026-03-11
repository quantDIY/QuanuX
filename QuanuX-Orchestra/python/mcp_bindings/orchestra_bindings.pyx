# Cython bindings for QuanuX Orchestra
__checksum__ = "4e8c3aa88f3b6839ee9754260fa204b6e646048cdf8b59563ee501703bf09c27"

from orchestra_constants cimport FixTag

cpdef int get_tag_value(FixTag tag):
    return <int>tag
