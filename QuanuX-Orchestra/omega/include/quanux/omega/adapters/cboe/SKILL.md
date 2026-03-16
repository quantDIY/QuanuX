---
description: AI Context Manual for QuanuX-Omega Cboe Adaptor Module
---

# `cboe` Agent Skill

## 1. Subdirectory Context & Purpose
The `adapters/cboe/` directory houses the mapping logic for the Cboe venue family. Implemented in Phase 10, this adapter tackles a unique challenge: representing a venue family (U.S. Equities + U.S. Options) via a solitary unified capability profile.

1. **Pre-Trade Map:** Identifies routing constraints when engaging with Cboe handlers across equities and options segments.
2. **Post-Trade Decoder:** Normalizes native telemetry, discerning options semantics (`short sell partial fill`) alongside equities semantics inside identical parsing boundaries.

## 2. Intentional Engineering
- **Unified Identity Abstraction**: Parses venue-family representations cleanly without polluting downstream logic with segment-specific boundaries.
- **Lifetime Preservation**: Extracts sub-venue or profile identifiers into `out_envelope.extensions._backing_opaque_extension_blob` ensuring memory durability without custom fields inside the `OmegaEventEnvelope`.
- **Symmetric Routing**: Demonstrates multi-asset semantics cleanly evaluate via the exact same normalized JetStream pipeline.

## 3. Rules of Interaction
1. **Respect Family Semantics**: Do not fracture this adapter into `cboe_equities/` and `cboe_options/` unless fundamentally conflicting ingress representations require it. Attempt to parse uniformly.
2. **Strict Identity Enforcement**: Define explicit venue mapping constraints and trap incompatible segments strictly within error bounds.
