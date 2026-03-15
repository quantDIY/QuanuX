---
description: AI Context Manual for QuanuX-Omega Evidence & Survival Tracing (`evidence/`)
---

# `evidence` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`evidence/`) manages the cryptographic retention of raw payloads. The `RawPayloadHash` generated here ensures absolute data integrity. 

When QuanuX-Annex invokes the `DecoderRing` on a raw NATS byte array, a fast 64-bit FNV-1a hash (or similar) of the *unparsed* payload is calculated and bound to the resulting `OmegaEventEnvelope`. 

## 2. Intentional Engineering (Traceability)
The Evidence structure guarantees that any decoded transaction can be mathematically proven against its raw venue packet stored in the cold-storage Parquet matrix.
- **Hash Retention**: We hash the raw bytes immediately upon traversal over the offline Cython/C++ boundary.
- **Why**: Reconstructing order books requires deterministic replay validation. The `RawPayloadHash` enables a Spreader test to rehydrate exact exchange scenarios by looking up the hash in the Zarr or DuckDB Lake.

## 3. Rules of Interaction
When accessing or modifying files inside `evidence/`:
1. **Never allocate heap space to hash**: The hashing must occur across the `const uint8_t* raw` parameter. 
2. **Deterministic Outputs**: If you write custom hashing implementations here, they must be endian-independent and mathematically deterministic. A payload arriving today mapping to Hash A must map to Hash A ten years from now.
3. Keep the evidence hashing logic entirely out of the execution layer. It is for post-execution reconciliation.
