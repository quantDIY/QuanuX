---
name: QuanuX-Annex Architecture
description: Operating procedures and architecture for the QuanuX-Annex Data Ingestion Engine and NATS JetStream mesh.
---

# QuanuX-Annex Architecture & Operations

## Overview
QuanuX-Annex is the high-performance, zero-allocation C++ data ingestion engine layer inside the QuanuX cluster. It is designed for maximum throughput, hardware-enforced thread affinity, and strict High Availability (HA) failover. It hooks directly into the `QUANUX.MARKET.TICK`, `QUANUX.EXECUTION.LOG`, and `QUANUX.SCHEMA.DRIFT` NATS subjects.

## Key Architectures

### 1. Zero-Allocation NATS JetStream C Client
The core ingestion loop avoids all Python/JVM garbage collection penalties by using the native `nats.c` library (`<nats/nats.h>`). It maps incoming byte streams directly into byte-aligned C++ structs defined in `include/quanux/annex/TelemetryExhaust.hpp`:
- `MarketTick`
- `ExecutionLog`
- `SchemaDriftEvent`

The client operates on an async callback model `NatsSubscriber.cpp`, preventing event loop blocking.

### 2. High Availability (HA) & Hardware Affinity
The QuanuX-Annex binary is deployed as a highly tuned `systemd` service (`quanux-annex.service`) to a dedicated edge compute node (e.g., DigitalOcean `c-2` class). It requires the `CLOUD_TARGET_DO` CMake flag to enable DigitalOcean `ZarrResolver_DO` behaviors.

It dynamically binds to the `10.10.10.5` internal VPC interface. It explicitly prevents on-node builds during normal operations to preserve CPU cycles (though Phase 11 testing utilized on-node compilation, this is documented Technical Debt).

### 3. Sentinel Protocol
Telemetry for the edge nodes is handled by Telegraf and Promtail. Telegraf is deployed via direct `.deb` package injection to bypass broken APT repositories, and its collection interval is dynamically injected via `quanuxctl habitat sentinel --telemetry-interval <seconds>`.

### 4. Mock Injection (Testing)
A Python mock injector (`tests/nats_injector.py`) uses the Python `struct` module to pack simulated payload structs (`<QIddIIB` format) into the exact byte-alignment required by the C++ engine over NATS, validating the wire layout without live market data.

## Phase 14: Hasura Federation & Streaming JSON Egress
The QuanuX-Annex Edge Node implements a `cpp-httplib` Remote Schema webhook listener for Hasura GraphQL.
- **AWS SigV4 UNSIGNED-PAYLOAD**: Bypasses heavy cryptographic SHA256 hashing during TLS `GET/PUT` transfers to DO Spaces natively in C++, significantly multiplying ingestion throughput.
- **Zero-Allocation Egress Streaming**: Rather than building massive `nlohmann::json` DOM trees representing gigabytes of ticks natively in RAM, `ZarrResolver_DO.cpp` directly intercepts `libcurl` streams, mapping them into raw memory chunks, and using `cpp-httplib`'s chunked provider to asynchronously stream the `snprintf` JSON representations directly back to Hasura. CPU bottlenecking is eradicated.

## Removed Technical Debt
- **Python Path Hacking Eradicated**: `quanuxctl/main.py` is safely packaged and executed.
- **NATS Decay Hazard Resolved**: Dynamic Keyring injection of `NATS_URL` is definitively hardened into permanent C++ object lifecycles, surviving all memory decaying scope drops.
- **Synchronous Egress Resolved**: The webhook thread-pool was aggressively scaled, and egress streaming prevents OOM kills on massive queries.

## Commands
```bash
# Deploy Payload to Edge Node
quanuxctl deploy -p annex_core -t quanux_annex_node --type cpp_binary

# Inject Mock NATS Data
python tests/nats_injector.py --url nats://10.10.10.5:4222 --subject QUANUX.MARKET.TICK --type tick --count 1000
```
