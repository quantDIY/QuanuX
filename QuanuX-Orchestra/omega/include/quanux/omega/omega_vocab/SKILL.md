---
description: AI Context Manual for QuanuX-Omega Semantic Taxonomy (`omega_vocab/`)
---

# `omega_vocab` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`omega_vocab/`) serves as the Rosetta Stone defining all recognized standardized enumerations (`enum class`) for the `OmegaEventEnvelope`. 

Here, you will find taxonomies such as `EventType`, `SourceType`, `NormalizedState`, and `NormalizedReasonCode`. This is the dictionary that QuanuX-Omega maps raw venue bytes into.

## 2. Intentional Engineering (Dual-Lifecycle Usage)
The vocabulary aligns strongly with ISO 20022 (FIX Orchestra) schema principles to ensure that terms like `NewOrder`, `PartiallyFilled`, or `Rejected` mean precisely the same thing regardless of the originating exchange. This taxonomy operates in two discrete phases:

1. **Pre-Trade / Pre-Compilation:** The orchestrator utilizes these dictionaries to map venue-specific execution constants against QuickFIX and OnixS structures before creating the trading binary.
2. **Post-Trade / Reconciliation:** Rather than passing around raw character codes (`'1'`, `'2'`, `'9'`) across the offline data lake, the Annex translates bytes into these explicit enums for off-path back-office accounting ops.

- **Microsecond Translation**: The vocabulary is natively built of pure `enum class` declarations mapped to explicitly sized integers (`uint8_t`, `uint16_t`).

## 3. Rules of Interaction
When defining new enumerations inside `omega_vocab/`:
1. **Maintain Size Boundaries**: Explicitly type your enum classes (e.g., `enum class EventType : uint8_t`) to ensure the `OmegaEventEnvelope` remains perfectly struct-packed. Tight cache lines equal faster offline analytics batches inside DuckDB.
2. **Global Representation**: Before adding a new state, ensure it is universally applicable (e.g., `Busted`, `Suspended`). If an enumeration is intensely unique to a single exchange (e.g., CME's exact match algorithm variant state), it belongs in the `survival/` tags, NOT in the global `omega_vocab`.
3. **Never parse on the hot path**: These vocabularies exist solely for `QuanuX-Omega` to output post-execution clarity to NATS observers. Do not wire `omega_vocab` into a pre-compiled Spreader execution strategy.
