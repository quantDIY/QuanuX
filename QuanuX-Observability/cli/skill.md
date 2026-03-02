---
description: Localized knowledge base for QuanuX-Observability cli extension (quanuxctl).
---

# QuanuX Observability: CLI Knowledge Base

The `cli/` directory houses the `quanuxctl obs` interface. This is the direct developer-to-mesh control plane.

## 1. The Stateless CLI Doctrine
- **Rule**: `quanuxctl` commands must never instantiate database connections to ValKey, DuckDB, OpenSearch, or any persistence layer.
- **Reasoning**: A Typer script running on an admin's local MacBook cannot reasonably be given direct TCP/SSL access to the core analytical clusters or high-speed hot state arrays. The CLI must remain decoupled.
- **Implementation**: The CLI acts entirely as a CNATS publisher. It translates terminal arguments into standardized JSON payloads and fires them to authorized control subjects (e.g., `quanux.control.obs.freeze`).

## 2. Command Mapping via CNATS
The C++ Daemons and Python Shadow Nodes establish asynchronous listeners to act upon these CLI instructions natively within the isolated secure cluster.

- `config-exchange`: Informs the global Exchange Registry to rotate epochs.
- `trigger-freeze`: Directly targets the C++ `SettlementDaemon`. The C++ process itself establishes the DuckDB C API binding and extracts the data. The CLI merely alerts it to do so.
- `cluster-health`: Subscribes dynamically to the JetStream execution firehose (`quanux.telemetry.vitals.>`) and aggregates the raw binary structs via the Cython bridge into a rich terminal view.

By enforcing this division, the user interface remains fundamentally disjoint from mechanical execution paths.
