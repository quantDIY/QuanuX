---
description: AI Context Manual for QuanuX-Omega COMEX Adaptor Module
---

# `comex` Agent Skill

## 1. Subdirectory Context & Purpose
The `adapters/comex/` directory contains the integration logic for the Commodity Exchange (COMEX). Built as part of Omega's Phase 9 expansion, it exercises the platform's metal-centric reporting semantics.

1. **Pre-Trade Map:** Provides expected shapes for COMEX data mapping into downstream trade runners.
2. **Post-Trade Decoder:** Converts independent `order_state` characters (`'N'`, `'F'`, `'R'`) that emulate matching engine variations distinct from standard CBOT logic.

## 2. Intentional Engineering
- **Zero-Copy Provenance**: Maps textual identifiers into thread-safe identity bounds. 
- **Genericity**: Enforces the Annex compatibility contract ensuring COMEX data flows directly into existing validation models without modification.

## 3. Rules of Interaction
1. **No External Imports**: The COMEX adapter must solely rely on Omega core objects and standard library features available without dynamic memory.
2. **Venue Boundaries**: Explicitly assert `venue_id = "COMEX"`. 
