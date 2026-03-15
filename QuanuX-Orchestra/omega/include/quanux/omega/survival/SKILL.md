---
description: AI Context Manual for QuanuX-Omega Tag Survival Component (`survival/`)
---

# `survival` Metadata Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`survival/`) safeguards venue-specific attributes that do not map elegantly into the universal `OmegaEventEnvelope`.

When QuanuX-Omega `adapters/` decode a payload, they map standard fields (Price, Side, Quantity) into the envelope core. Any exotic or proprietary exchange tags (e.g., CME's sequence resets or obscure routing tags) that are unmapped but valuable are caught in the `survival::TagValue` structure.

## 2. Intentional Engineering (No Data Left Behind)
We never drop unmapped venue metadata. However, we absolutely refuse to bloat the main `OmegaEventEnvelope` struct with permutations of venue-specific flags.
- **The Survival Vector**: Unmapped tags are appended to a minimal C++ vector (representing field ID and byte value).
- **Offline Querying**: Because `QuanuX-Omega` operates **off-the-hot-path**, this appending logic does not inflict CPU cache misses or heap-allocation lag on the execution engine.

## 3. Rules of Interaction
When developing inside `survival/`:
1. **No Core Bloat**: Do not propose moving a field from the Survival bag into `omega_core` unless the attribute is universally represented across all 8+ integrated venues.
2. **Byte Conservation**: Values retained in the Survival bag should use memory-safe views (`std::string_view`) or tight byte arrays, limiting excessive memory ballooning when decoding 2-million payload bursts from the Annex tap.
3. This is purely a translation reconciliation mechanism. Survival logic is strictly forbidden inside the Spreader node.
