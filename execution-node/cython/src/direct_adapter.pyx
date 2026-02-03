# distutils: language = c++
# cython: language_level = 3

from adapter cimport Adapter
import asyncio

# We rely on dynamic import or existing extensions being in path
# cimporting them directly might require pxd files for them.
# For v1, we use python object wrapping (duck typing) for flexibility.

cdef class DirectAdapter(Adapter):
    """
    Wraps a native Cython extension (TopstepClient/RithmicEngine) 
    and exposes it via the standard Adapter interface.
    """
    cdef public object client
    cdef public str venue

    def __init__(self, str venue, object client):
        super().__init__(name=venue)
        self.venue = venue
        self.client = client # e.g. TopstepClient instance

    async def connect(self):
        # Assumes client has login logic handled or exposed
        # For topstep, login is async.
        # We might need config to pass credentials here or in client init.
        pass

    async def subscribe(self, list symbols):
        # Dispatch to client specific logic
        # if Topstep...
        pass

    async def place_order(self, dict order):
        # Dispatch
        return await self.client.place_order(order["accountId"], order)
