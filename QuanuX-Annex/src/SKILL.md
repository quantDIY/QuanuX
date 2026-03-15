---
description: QuanuX-Annex Source Code Root
---

# QuanuX-Annex Base Architecture

The `src/` directory aggregates the two primary processing forks of the QuanuX C++ Edge Node:

1. **The Ingestion Path (`resolvers/`)**: High-frequency bindings to NATS JetStream dropping binary arrays into Zarr RAM buffers, and asynchronous flushers using native C `libcurl` (with `curl_multi`) transferring state to DigitalOcean/AWS object storage.
2. **The Retrieval Path (`federation/`)**: The C++ webhook listener running on `cpp-httplib` fielding Hasura Remote Schema execution queries, retrieving and decompressing the Zarr chunks back into raw `MarketTick` structs, and finally streaming the zero-allocation JSON back to the Hasura master engine.

## Agent Mandates
- Do not introduce Python runtime dependencies or JVM bridges. `quanux-annex` is strictly C/C++ native metal.
- Refer strictly to the memory-aligned padding rules inside `include/quanux/annex/TelemetryExhaust.hpp`. Python struct packers (`<QIddIIB`) must perfectly map to the C++ types.

When generating subsequent integrations (e.g., Azure or GCP analytics), stick to the blueprint mapped in `ZarrResolver_DO.cpp`.
