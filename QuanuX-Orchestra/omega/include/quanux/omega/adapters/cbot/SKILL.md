---
description: AI Context Manual for QuanuX-Omega CBOT Adaptor Module
---

# `cbot` Agent Skill

## 1. Subdirectory Context & Purpose
The `adapters/cbot/` directory maps the Chicago Board of Trade execution states into Omega's universal abstractions. Integrated during Phase 9 to broaden U.S. coverage across list derivatives ecosystems. 

1. **Pre-Trade Map:** Identifies routing and parsing metadata expected from CBOT handlers.
2. **Post-Trade Decoder:** Transposes integer-mapped SBE (Simple Binary Encoding) enumerations into precise `EventType` states within the `OmegaEventEnvelope`.

## 2. Intentional Engineering
- **Generic Alignment**: The adapter guarantees CBOT executions successfully evaluate through the exact same NATS JetStream validity checks as CME or IBKR.
- **Identity Parsing**: Uses distinct routing identifiers (`ProductGroup` mappings) parsed firmly into the `_backing_string` elements of `OmegaEventIdentity`.

## 3. Rules of Interaction
1. **Never Abstract the Source**: Adapt native integer schemas faithfully without lossy approximations. Store unknowns in extensions.
2. **Deterministic Processing**: Maintain zero memory allocations to keep parsing instantaneous. 
