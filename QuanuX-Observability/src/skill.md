---
description: Localized knowledge base for QuanuX-Observability src (C++ Core Execution Plane).
---

# QuanuX Observability: C++ Source Knowledge Base

This document enforces the strict execution laws for the `QuanuX-Observability/src/` directory. Code here represents the Sovereign C++ Engine and its native integrations.

## 1. The C++20 Mandate
- **Rule**: All source must conform strictly to C++20 standards.
- **Reasoning**: To prevent legacy legacy code pollution (e.g., C++11 paradigms) that compromises performance and memory coherence.
- **Implementation**:
  - Prefer modern memory management constraints but prioritize zero-copy execution over pointer safety where hot-paths demand it.
  - Utilize `<format>` and `<span`> constructs where appropriate.
  - No `std::shared_ptr` wrapping over memory buffers that must be blasted across the wire immediately. Make allocations flat and single-owned.

## 2. The QuanuX Consistency Doctrine 
- **Rule**: Do NOT introduce secondary message brokers, standard library socket streams, or JSON parsers.
- **Networking**: `nats.h` (The CNATS C Client) is the ONLY allowed transport mechanism.
- **Analytics Database**: `duckdb.h` (The DuckDB C API) is the ONLY allowed analytical endpoint within C++.
- **Serialization**: `flatbuffers.h` is the ONLY allowed memory structuring construct for network payloads.

## 3. TelemetryPublisher Mechanics
- **Function**: Takes standard execution integers (`int64`, `uint64`), wraps them in the `TradeExecution` FlatBuffer, strings them into a `TelemetryEnvelope`, and pipes the contiguous memory pointer directly to `natsConnection_Publish`.
- **Zero-Copy**: The builder's core buffer is accessed via `.GetBufferPointer()` and handed directly to CNATS for kernel dispatch.

## 4. SettlementDaemon Architecture
- **Function**: Asynchronously listens to `quanux.registry.state` over a CNATS JetStream subscription.
- **DuckDB Integration**: Upon detecting a `FROZEN_SETTLEMENT` market status for an exchange (e.g., LSE at 16:30 GMT), it executes a native DuckDB SQL query via `duckdb_query()` using the C API. 
- **Parquet Offloading**: The C API is commanded to pull the hot state and natively `COPY TO` cold Parquet storage (e.g., an S3 or DigitalOcean Spaces bucket) directly from the C++ process. No Python intervention is permitted here.
