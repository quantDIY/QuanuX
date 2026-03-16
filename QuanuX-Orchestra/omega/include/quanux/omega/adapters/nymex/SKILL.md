---
description: AI Context Manual for QuanuX-Omega NYMEX Adaptor Module
---

# `nymex` Agent Skill

## 1. Subdirectory Context & Purpose
The `adapters/nymex/` directory translates the New York Mercantile Exchange executions. Introduced during Phase 9 to assure the internal abstract mapping framework handles broad commodity venue types.

1. **Pre-Trade Map:** Identifies required structural bounds for compiling gateway parsers targeting NYMEX.
2. **Post-Trade Decoder:** Implements a custom `trade_id` hash function to cleanly allocate `ids::EventId` while mapping NYMEX execution metadata into `OmegaEventEnvelope`.

## 2. Intentional Engineering
- **Precision Auditing**: Includes necessary Capability Warnings defining temporary `double` abstractions within the NYMEX namespace, matching `types::Price` framework deprecations.
- **Resilient Provenance**: Protects downstream logic from parse failures by encapsulating bad memory blocks natively into Dead-Letter paths.

## 3. Rules of Interaction
1. **Respect Temporary Bounds**: Continue ensuring NYMEX mapping surfaces emit deprecation warnings where provisional analytics strings exist.
2. **No Iterative Parsing**: Ensure mapping runs in pure `O(1)` runtime leveraging native C-struct decoding.
