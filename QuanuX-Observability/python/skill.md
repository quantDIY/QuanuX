---
description: Localized knowledge base for QuanuX-Observability python guest nodes.
---

# QuanuX Observability: Python Guest Node Knowledge Base

This directory contains the Shadow Node architecture. As defined by the Master Blueprint, Python is explicitly forbidden from touching core trading memory or analytical lifting. The Shadow Node serves entirely as an asynchronous traffic cop for observability data.

## 1. The Cython Boundary
- **Rule**: NEVER use `struct.unpack()` or the native Python `flatbuffers` library on the hot message ingestion path.
- **Reasoning**: The Python object lifecycle within the interpreter loop cannot keep up with millions of parsed binary messages. Trying to unpack an HFT stream securely with native Python will crash the node.
- **Implementation**: We assume the existence of an optimized C-extension (`quanux_cython_bridge`) compiled specifically to map FlatBuffer raw bytes into pre-allocated Python dictionaries instantaneously. The ingestion loop relies strictly on this non-blocking call.

## 2. The Non-Blocking Doctrine
- **Rule**: The NATS `async` callback (`on_telemetry`) MUST NOT block. No network requests, no database saves.
- **Reasoning**: NATS JetStream will enforce backpressure if the subscription callback stalls. Blocking the event loop via a slow HTTP request (like to OpenSearch) will cause memory blowouts and dropped connections.
- **Implementation**: The ingestion callback strictly parses the payload via Cython, updates local `prometheus_client` memory counters synchronously, and uses `queue.put_nowait()` to dump the payload references into `asyncio.Queue` structures.

## 3. The Tri-Partite Dispatching Model
Python decouples ingestion from I/O entirely using dedicated background asyncio tasks.
- **ValKey (Hot Buffer)**: Dedicated dispatcher builds Redis Pipelines to batch-update keys sequentially rather than individually. This maximizes throughput into the cache.
- **Prometheus (Vitals)**: Updated synchronously as counters/gauges are memory-only abstractions.
- **OpenSearch (Forensics)**: Dedicated dispatcher buffers high-volume logs and performs periodic HTTP `_bulk` inserts, completely separated from core NATS processing limits.
