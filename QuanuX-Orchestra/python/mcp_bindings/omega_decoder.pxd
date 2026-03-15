# Cython wrappers detailing the C++ classes and structures we need to bind
cdef extern from "quanux/omega/translation/decoder_ring.hpp" namespace "quanux::omega::translation":
    cpdef enum class VenueId(unsigned short):
        Unknown = 0
        CmeMdp3 = 1
        NasdaqOuch = 2
        IbkrOnixs = 3

cdef extern from "quanux/omega/omega_core/omega_event_envelope.hpp" namespace "quanux::omega::core":
    cdef cppclass OmegaEventTime:
        pass
    
    cdef cppclass OmegaEventEnvelope:
        OmegaEventEnvelope() except +
        
        # In actual production you would map the fields here to read them from Python, e.g.
        # vocab::EventType event_type
        # vocab::NormalizedState normalized_state
        # etc.

cdef extern from "quanux/omega/survival/tag_value.hpp" namespace "quanux::omega::survival":
    cdef cppclass TagValue:
        TagValue() except +
        unsigned int tag_id
        # We must expose methods to get the string_view data out safely in cython if needed.

cdef extern from "quanux/omega/translation/decoder_ring.hpp" namespace "quanux::omega::translation":
    cdef cppclass DecoderRing:
        @staticmethod
        bint decode(VenueId venue_id, const unsigned char* raw_buffer, size_t buffer_len, OmegaEventEnvelope& out_envelope, TagValue& out_survival_tag) except +
