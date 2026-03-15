---
description: AI Context Manual for QuanuX-Omega Core Structures (`omega_core/`)
---

# `omega_core` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`omega_core/`) contains the foundational struct definitions for the `OmegaEventEnvelope`, `OmegaEventIdentity`, `OmegaEventLinkage`, and `OmegaEventTime`. 

These structures outline the **explicit semantic state** of data functioning across two separate lifecycles:
1. **Pre-Trade / Pre-Compilation**: The taxonomy defined here acts as the rigid mapping anchor used by `standardizer_cli` before execution kernels are even compiled. It bridges venue-specific definitions (like QuickFIX/OnixS tags) down into absolute C++ constants.
2. **Post-Trade / Reconciliation**: These structures represent the final decoded state within the QuanuX ecosystem after a raw NATS telemetry message has been ingested by `Annex` without intersecting the hot-path.

## 2. Intentional Engineering (Do Not Break)
The files in this directory are heavily optimized to prevent unnecessary memory allocations during ingestion:
- **No Dynamic Heaps (`std::string_view` over `std::string`)**: The structures utilize `std::string_view` extensively to map directly over raw SBE (Simple Binary Encoding) buffers or JSON payloads without triggering memory copying. 
- **Immutable Semantics**: These structs are not built to be "manipulated" during runtime. They are built to act as strict, mathematically verifiable envelopes for regulatory storage and reconciliation.

## 3. Rules of Interaction
When generating code or reading from `omega_core`:
1. **Never mutate core headers to accommodate unique venue logic**: If a specific exchange like CME drops a field, do not add `cme_specific_field` to `OmegaEventEnvelope`. Custom or unmapped fields belong entirely in the `survival/` metadata vectors, NOT the core structures.
2. **Never invoke `omega_core` structs on the 59ns Hot Path**: The Spreader trading nodes are designed to run without envelope wrapping. The structures here are invoked exclusively by the `QuanuX-Annex` components listening strictly via offline NATS tap.
3. If an LLM suggests using `std::string` inside `omega_event_identity.hpp`, **reject it**. Enforce `std::string_view` to preserve zero-copy boundaries.
