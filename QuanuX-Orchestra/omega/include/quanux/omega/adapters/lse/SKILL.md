---
description: AI Context Manual for QuanuX-Omega LSE Adaptor Module
---

# `lse` Agent Skill

## 1. Subdirectory Context & Purpose
The `adapters/lse/` directory translates execution states from the London Stock Exchange (LSE) into Omega's universal structural bounds. This validates Omega's capability to ingest international equity market rules without demanding structural rewrites of the core platform.

1. **Pre-Trade Map:** Acts as a linkage blueprint for integrations targeting LSE Native or FIX interfaces.
2. **Post-Trade Decoder:** Transposes LSE execution types (which use padded strings like `'1 '` for Partial) into `vocab::NormalizedState` efficiently.

## 2. Intentional Engineering
- **Semantic Evasion & Normalization**: Maps distinct international equity execution patterns gracefully without causing logic branches inside the generic routing network.
- **Identity Parsing**: Extracts string-based matching states via bounds-checked scans to populate `_backing_string` instances, protecting pointer lifetime bounds.

## 3. Rules of Interaction
1. **Never Allocate**: Protect the instantaneous parsing mandate by parsing arrays strictly via in-place casting and zero-copy string views.
2. **Generic Annex Output**: Avoid creating downstream routing complexity; map edge-cases safely into `extension_blob` tracking for diagnostic replay.
