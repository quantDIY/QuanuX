---
description: AI Context Manual for QuanuX-Omega Translation Registry (`translation/`)
---

# `translation` DecoderRing Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`translation/`) is the heart of the QuanuX-Omega engine. It contains the `DecoderRing` registry (`decoder_ring.hpp`), which acts as the universal translator linking raw binary/JSON telemetry buffers into `OmegaEventEnvelope` structures.

## 2. Intentional Engineering (The Sovereign Pivot)
The QuanuX-Omega engine is architected to exist **completely separated from the Execution nodes**. It functions across two lifecycles:
- **Pre-Trade / Pre-Compilation:** The registry maps venue capabilities to execution handlers (QuickFIX, OnixS) during the compilation phase, establishing zero-latency `constexpr` bounds before strategies deploy.
- **Post-Trade / Reconciliation:** In standard architectures, telemetry translation is done locally on the execution hub, causing algorithmic jitter. **In QuanuX:** The Tier 3/Tier 4 engines execute pure math and spit raw venue bytes onto NATS JetStream. The off-path `Annex` engine ingests those raw bytes and invokes `DecoderRing::decode(venue_id, raw_ptr, raw_size)` for accounting and back-office ops.

## 3. Rules of Interaction
When extending or interacting with `translation/`:
1. **The Hot-Path Ban**: Never include `decoder_ring.hpp` inside anything related to `QuanuX-Spreader` or the algorithmic nodes. Do not propose architectural optimizations that combine execution and decoding.
2. **Stateless Functions**: The `DecoderRing` must remain completely stateless. Do not add caching layers, class attributes, or mutex locks to the DecoderRing. It is a pure, immutable mapping function taking `(venue_id, const uint8_t*, size_t)` and emitting an `OmegaEventEnvelope`.
3. **Pointer Arithmetic Protection**: Keep the input types strictly as `const uint8_t* raw` and `size_t length`. This allows Cython and native C++ to pass contiguous memory blocks blindly across boundaries without deserialization overhead.
