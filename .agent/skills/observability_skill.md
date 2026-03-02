---
name: QuanuX Observability Plane Architecture
description: The absolute architectural laws governing the QuanuX Data & Telemetry Plane (Operation Panopticon Genesis).
---

# QuanuX Observability Plane Architecture: The Master Blueprint

This document defines the strict, immutable laws of the QuanuX Observability and Quantitative Storage Plane. Any AI Agent, Developer, or Architect operating within this codebase must adhere to these polyglot boundaries. Failure to do so will result in structural latency injection and system crashes.

## 1. The Polyglot Rule
The QuanuX Mesh is a strictly divided polyglot environment.
* **C++ is the Sovereign Engine.** All core execution parsing, risk checks, algorithmic routing, and deterministic heavy lifting (like DuckDB analytical pulls) MUST occur in C++.
* **Python is the Guest Layer.** Python is used *only* as an asynchronous traffic cop or a high-level coordination endpoint. Python nodes must never touch the core trading memory directly or perform synchronous IO operations that block event loops.

## 2. The CNATS Global Mesh Mandate
* **No HTTP Between Nodes**. The only acceptable inter-node communication mechanism is the CNATS Client over NATS JetStream. 
* Do not attempt to use raw TCP sockets, ZeroMQ, Kafka, or HTTP REST APIs for engine-to-engine or engine-to-shadow communication.
* Never use the phrasing "Standard NATS" in execution contexts. It is CNATS natively or `nats-py` within the Shadow Nodes.

## 3. The FlatBuffer Zero-Copy Rule
All structured telemetry and execution data is serialized exclusively as **FlatBuffers**. 
* JSON parsing on the hot execution plane is structurally banned.
* Data types must be institutional: `int64` for fixed-point money math, `uint64` for identifiers. No `float` or `double` allowed for money. No variable-string lookups that break CPU cache locality.

## 4. The DuckDB C API Contract
The Quantitative Vault relies on DuckDB natively exporting into Parquet formats.
* **WARNING**: Never use the `duckdb` Python module anywhere in this repository.
* All DuckDB analytical queries and `COPY TO` Parquet offloading commands MUST be written inside the C++ Sovereign Engine (`QuanuX-Observability/src/SettlementDaemon.cpp`) using native `duckdb.h` C API structures (`duckdb_state`, `duckdb_query`).

## 5. The Python Cython Boundary & Non-Blocking Vow
The Python Shadow Node (`QuanuX-Observability/python/shadow_node.py`) is responsible for routing telemetry.
* **WARNING**: Pure Python `struct.unpack()` or native `flatbuffers` libraries are banned on the ingestion path. The node expects a pre-compiled Cython extension module (`quanux_cython_bridge.parse_telemetry()`) to unmarshal zero-copy memory blocks instantly.
* The async NATS callback must be ruthlessly fast. It is explicitly forbidden from making blocking network requests. Items must be offloaded into local `asyncio.Queue` structures for background, bulk-batched execution (into ValKey pipelines or OpenSearch Bulk HTTP endpoints).

## 6. The Air-Gapped CLI (quanuxctl)
The `quanuxctl obs` CLI (`QuanuX-Observability/cli/obs_commands.py`) is perfectly stateless.
* The CLI must never instantiate database connections (no Redis, no DuckDB).
* It communicates entirely by publishing control payloads (e.g., `quanux.control.obs.freeze`) to the CNATS mesh, which the internal C++ daemons consume and execute.
