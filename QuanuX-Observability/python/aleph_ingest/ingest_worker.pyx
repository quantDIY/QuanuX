# distutils: language = c++
# distutils: sources = worker_engine.cpp

from libcpp.string cimport string

cdef extern from "worker_engine.h":
    string format_payload(const char* data, size_t len) except +

def process_jetstream_payload(bytes payload):
    """
    Takes a raw JSON payload (bytes) directly from JetStream and securely 
    converts it to the Aleph zero-space Markdown Standard via native C++ 
    JSON bindings, bypassing Python GIL manipulation loops in the hot path.
    """
    cdef string result = format_payload(<const char*>payload, len(payload))
    return result.decode('utf-8')
