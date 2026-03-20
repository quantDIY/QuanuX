---
description: AI Context Manual for QuanuX-Omega NASDAQ Adaptor Module
---

# `nasdaq` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`adapters/nasdaq/`) contains the specialized mapping logic for the NASDAQ exchange venue. It was introduced during Phase 7 of Omega’s evolution to demonstrate robust broad ingestion from major U.S. equities, and subsequently hardened to manage live trading boundaries securely.

The adapter encapsulates two primary responsibilities:
1. **Directory-Backed Locate Resolution**: Storing sequentially mapped stock locates in an O(1) `std::array` to consistently hydrate `OmegaEventIdentity`.
2. **Operational Source of Truth**: Tracking and gating ingress events through explicit `RegistryReadiness` bounds (ColdStart, PartialPreload, Ready, Degraded, RecoverySync).

## 2. Intentional Engineering (Do Not Break)
The `nasdaq_adapter` embodies strict performance and semantic adherence, aggressively hardened to fail-closed during instability:
- **Lifetime Safety**: It uses `_backing_string` instances within `OmegaEventEnvelope` instead of transient or `static` pointers, guaranteeing thread safety during concurrent parses.
- **Fail-Closed by Default**: Parsing incomplete or malformed telemetry unconditionally fails. Execution routing paths block universally when `!is_ready()`.
- **Timestamp-Aware Overwrite**: ITCH 'R' events are directory payloads. They populate the registry securely using strictly forward-moving nano-timestamp ingestion logic. Older replayed timestamps natively reject.
- **Degradation Differentiation**: `SequenceGap` and `MulticastDrop` require sequencer target convergences securely. `HeartbeatTimeout` recovers instantly on live traffic resume. `OperatorOverride` firmly prohibits all automated auto-healing transitions seamlessly.

## 3. Rules of Interaction & Changing Doctrine Safely
When interfacing or expanding the NASDAQ adapter, ensure Annex genericism remains intact. If you need to change core doctrine:
1. **Changing Timestamp-Aware Overwrite**: To permit out-of-order execution (e.g., late 'R' events for offline settlement tracking), do **NOT** modify the core `declare_locate()` logic. Create a parallel `declare_locate_offline()` path bypassing the `last_update_nanos` check specifically for backtesting.
2. **Changing RecoverySync Exits**: If business rules demand waiting for a "Clear to Trade" operator packet, **DO NOT** delete the target sequence catch-up evaluation in `check_catchup_completion()`. Add the network packet criteria *as an AND condition* to safely maintain exact tracking limits mathematically.
3. **Changing Operator Overrides**: To enable automatic an "Operator Timeout" unblocking mechanism, do not mutate numeric enum maps. Inject a time evaluation immediately following the `OperatorOverride` conditional check natively instead of deleting it.
