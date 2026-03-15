# Cython bindings for QuanuX Omega Decoder Ring
# Built for strict bare-metal deployment (Ubuntu/Debian) on DigitalOcean

from omega_decoder cimport VenueId, DecoderRing, OmegaEventEnvelope, TagValue
from libc.stdint cimport uint8_t

cdef class PyDecoderRing:
    """ Python entrypoint for the Annex to access the stateless Omega DecoderRing """

    @staticmethod
    def decode_payload(int venue_int_id, bytes payload):
        """
        Takes a raw Python bytes object acting as our network ring buffer input,
        casts it strictly off-heap to a C++ pointer, and decodes it.
        """
        cdef VenueId venue = <VenueId>venue_int_id
        cdef const unsigned char* c_payload = <const unsigned char*>payload
        cdef size_t p_length = len(payload)
        
        # Stack allocated structs to receive zero-copy mapping
        cdef OmegaEventEnvelope env
        cdef TagValue surv
        
        # Invoke stateless C++ decoder
        cdef bint success = DecoderRing.decode(venue, c_payload, p_length, env, surv)
        
        # In a real system, you would construct a Python dictionary or namedtuple
        # from the `env` struct fields and return it to the pure Python caller here,
        # but the zero-copy C++ processing has already successfully executed at this line.
        
        return success
