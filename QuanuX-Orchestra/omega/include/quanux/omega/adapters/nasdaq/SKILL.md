---
description: AI Context Manual for QuanuX-Omega NASDAQ Adaptor Module
---

# `nasdaq` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`adapters/nasdaq/`) contains the specialized mapping logic for the NASDAQ exchange venue. It was introduced during Phase 7 of Omega’s evolution to demonstrate robust broad ingestion from major U.S. equities.

1. **Pre-Trade Map:** Acts as a linkage blueprint for handler integration prior to compiled execution.
2. **Post-Trade Decoder:** Translates NASDAQ-specific binary telemetry (e.g., execution reports) and states into the universal `OmegaEventEnvelope`.

## 2. Intentional Engineering
The `nasdaq_adapter` embodies strict performance and semantic adherence:
- **Lifetime Safety**: It uses `_backing_string` instances within `OmegaEventEnvelope` instead of transient or `static` pointers for capturing protocol-specific strings, guaranteeing thread safety during concurrent parses.
- **Zero-Allocation Decoding**: Employs direct pointer casting to ingest structural definitions without dynamic heap allocations.
- **Semantic Mismatch Handling**: Unmapped states or missing routing keys natively parse into `ParseStatus::Error`, forcing the event cleanly into dead-letter routing within Annex, avoiding downstream corruption.

## 3. Rules of Interaction
When interfacing or expanding the NASDAQ adapter:
1. **Never Pollute the Core**: NASDAQ-specific tags or metadata must be sequestered into `OmegaEventExtensions.venue_native_event_code` or `tags`, rather than demanding a core schema update.
2. **Respect Identity Separation**: Explicitly assign `venue_id = "NASDAQ"`. Do not attempt to assign routing or counterparty identifiers here.
3. **Annex Genericism**: Ensure any updates map flawlessly into existing NATS JetStream validation structures without demanding branching inside NATS.
