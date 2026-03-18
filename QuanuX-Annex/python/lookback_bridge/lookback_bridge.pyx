# distutils: language = c++
# distutils: sources = lookback_engine.cpp

from libcpp.string cimport string

cdef extern from "lookback_engine.h":
    string execute_zero_copy_read(const string& ipc_path, const string& query) except +

def remote_schema_query(str ipc_path, str query):
    """
    Called by the Hasura Action Handler. Binds directly to the C++ layer 
    to execute a zero-copy read against the Arrow IPC matrix, returning 
    the Aleph Markdown Standard payload.
    """
    cdef string cpp_path = ipc_path.encode('utf-8')
    cdef string cpp_query = query.encode('utf-8')
    
    cdef string result = execute_zero_copy_read(cpp_path, cpp_query)
    return result.decode('utf-8')
