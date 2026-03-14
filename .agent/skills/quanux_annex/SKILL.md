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

## Phase 11 & Phase 12 Deployment Technical Debt
During the initial Phase 11 payload drop on DigitalOcean, the following tactical shortcuts were taken which must be audited:
- **On-Node Compilation:** The `c-2` node compiled the C++ binary. This expands attack surface and wastes CPU. Must be moved to CI/CD.
- **Python Path Hacking:** `quanuxctl/main.py` modifies `sys.path` to resolve modules rather than relying on a formal Python package.
- **APT Repo Bypass:** Telegraf was installed statically, breaking auto-updates.

## Commands
```bash
# Deploy Payload to Edge Node
quanuxctl deploy -p annex_core -t quanux_annex_node --type cpp_binary

# Inject Mock NATS Data
python tests/nats_injector.py --url nats://10.10.10.5:4222 --subject QUANUX.MARKET.TICK --type tick --count 1000
```
