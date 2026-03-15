---
description: AI Context Manual for QuanuX-Omega Adaptor Modules (`adapters/`)
---

# `adapters` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`adapters/`) contains the specialized mapping logic for individual exchanges and venues (e.g., `cme/`, `ibkr/`, `topstep/`). It is a dual-purpose translation bridge:
1. **Pre-Trade Map:** Used prior to strategy compilation to anchor venue definitions to specific order/data handlers (QuickFIX, OnixS).
2. **Post-Trade Decoder:** Translates the specific memory offsets of a venue's unmapped raw telemetry payload into universal `OmegaEventEnvelope` properties for back-office and accounting validation.

## 2. Intentional Engineering
Rather than parsing fields out of order, adapters in QuanuX-Omega follow strict memory mapping principles:
- **Zero-Allocation Decoding**: Adapters generally cast pointers directly over incoming buffers (e.g., CME's Simple Binary Encoding MDP3 buffers) rather than parsing them into intermediate JSON maps. 
- **O(1) Map Lookups**: Adapter behavior is fully deterministic. Translating from a venue's byte header directly into `OmegaEventEnvelope` structures without performing iteration. 

## 3. Rules of Interaction
When creating a new adapter in this directory:
1. **Folder Sovereignty**: Always create a distinct subdirectory (`adapters/[venue_name]`).
2. **Pure Pointers**: Your interface must take `(const uint8_t* raw_data, size_t length)`. Do not accept abstracted classes or nested dictionaries.
3. **No Execution Logic**: An adapter's only purpose is data transformation for the **Post-Execution** layer. Adapters must never embed logic to send orders, calculate risk, or listen to sockets.
4. **Never Block or Sleep**: Do not invoke `sleep()`, Network I/O, or OS disk operations from within an adapter. The adapter is invoked over a shared memory buffer and must complete the translation instantaneously.
