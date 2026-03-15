---
description: AI Context Manual for QuanuX-Omega Identity Mapping (`omega_ids/`)
---

# `omega_ids` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`omega_ids/`) contains the foundational structures used to globally identify entities within the QuanuX Spreader ecosystem post-execution. 

When a payload arrives from a venue, it often uses local mapping identifiers (e.g., Integer Sequence numbers or truncated strings for `clOrdId` or `Account`). The Decoded `OmegaEventIdentity` object handles the transformation and retention of these IDs for offline regulatory, risk, and P&L analysis.

## 2. Intentional Engineering (FIGI Enforcement)
The QuanuX ecosystem strictly rejects string-based ticker lookups on the hot-path (e.g., translating "ESM4" to "ES M4").
- **The Global Unifier**: All parsed IDs must adhere to the **FIGI (Financial Instrument Global Identifier)** standard whenever possible. 
- **Integer IDs > String Maps**: For high-velocity accounts and execution components, the system utilizes raw 64-bit integers.

## 3. Rules of Interaction
When interfacing or developing inside `omega_ids/`:
1. **Never use `std::string`** inside identity structures: Rely on `std::string_view` mapped directly over the binary packet, or `std::array<char, N>` arrays for bounded storage limits.
2. Ensure you understand that Identity resolving via the `DecoderRing` does not impact front-line trading speeds because Omega is partitioned behind the `QuanuX-Annex` layer.
3. If LLM outputs attempt to use `unordered_map<string, ...>` to resolve identifiers inside `omega_ids/`, categorically reject the proposal. Identify parsing must be deterministic and zero-allocation.
