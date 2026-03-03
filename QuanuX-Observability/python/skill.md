---
description: QuanuX Observability - Python Shadow Node Matrix Laws & Ingestion
---

# QuanuX Observability: The Python Shadow Node

## 1. The Tri-Partite Decoupling Law
Python must NEVER operate within the critical execution path of the C++ Sovereign Engines. The Observer effect must be absolute zero.
- **Rule**: The Python Shadow Node strictly operates Out-of-Band (OOB) via the `10.10.10.x` JetStream Firehose (`nats://10.10.10.2:4222`).
- **Rule**: Python telemetry ingestion routines must use asynchronous bulk ingestion patterns (`asyncio`, Cython bridges) to drain JetStream without blocking. 
- **Rule**: The JVM is permanently banned from the Execution Plane (The Edge Nodes). It is strictly permitted on the Observability Plane (The Panopticon) solely to run the OpenSearch Forensic Ledger. All analytics, routing, and indexing are strictly Python/Cython-based.

## 2. Ingestion Routing & Storage
- **Protocol**: High-frequency metric streams (`QUANUX.PULSE.>`, `QUANUX.TELEMETRY.>`) are scraped natively from JetStream.
- **Transformation**: Raw FlatBuffer bytes are unpacked via the `libflatbuffers-dev` Cython bindings to bypass the Python interpreter loop penalty.
- **Storage**: Unpacked metrics are immediately batched and flushed to the internal Panopticon Ledger (Async OpenSearch cluster) strictly using the low-level `opensearch-py` asynchronous client via `helpers.async_bulk`. No synchronous database writes are permitted, rigorously preserving the Tri-Partite Decoupling Law.

## 3. The Visualization Boundary
The Python application provides aggregated dashboard summaries and GraphQL subscriptions for the "Dumb Dome" frontend clients (React). The UI does not query the engines; the UI queries the Shadow Node's internal aggregation states.
