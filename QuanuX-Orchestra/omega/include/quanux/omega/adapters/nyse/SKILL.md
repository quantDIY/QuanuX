---
description: AI Context Manual for QuanuX-Omega NYSE Adaptor Module
---

# `nyse` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`adapters/nyse/`) houses the adapter code supporting the New York Stock Exchange. It mirrors NASDAQ's Phase 7 integration, serving as the other major pillar in broad U.S. equities ingestion mapping.

1. **Pre-Trade Map:** Identifies fields intended for hardware handlers (like OnixS or QuickFIX).
2. **Post-Trade Decoder:** Normalizes native NYSE binary executions into `OmegaEventEnvelope` structures, proving the abstraction handles varied U.S. matching engine semantics natively.

## 2. Intentional Engineering
The `nyse_adapter` leverages Omega’s boundary protections:
- **Thread Safety**: It preserves native extension bytes via inline `.extensions._backing_opaque_extension_blob` arrays rather than thread-unsafe static buffers.
- **Strict Capabilities**: Exposes a `SourceCapabilityProfile` which accurately communicates the adapter's adherence to semantic boundary tests and its reliance on provisional numerics (if any) during the parsing flow.
- **Dead-Letter Resiliency**: Safely traps missing IDs or unexpected binary opcodes by mapping to `ParseStatus::Error`, successfully directing invalid traffic away from fast-path processors.

## 3. Rules of Interaction
When modifying or reviewing the NYSE adapter:
1. **Preserve Native Integrity**: Unrecognized execution opcodes should map safely to extensions and force an error parsing status if critical to lifecycle.
2. **Maintain Genericism**: Maintain the identical outbound payload contract enabling Annex Consumer Router to handle the NYSE events without specific switch statements.
3. **No Network Access**: The adapter is purely computational and in-memory. It cannot use sockets, sleep, or file hooks.
