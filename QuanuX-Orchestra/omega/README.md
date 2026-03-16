# QuanuX-Omega Protocol

QuanuX-Omega is the sovereign semantic parsing and normalization boundary forming the foundation of the QuanuX execution architecture. The platform assumes that raw execution drops (QuickFIX, OnixS, WebSocket Streams, etc.) are unmapped and chaotic. Omega strips away transport complexities, applying zero-allocation mappings to output canonical `OmegaEventEnvelope` records.

Omega powers two phases of the trade lifecycle:
1. **Pre-Deployment / Pre-Trade Map:** Before an algorithmic strategy compiles, Omega defines exactly which raw FIX tags or binary SBE structures the execution handlers must extract. 
2. **Post-Trade Decoder:** As raw buffer execution reports hit the internal NATS JetStream mesh, Omega reads the streams instantaneously, deciphering bytes into back-office and accounting-grade metrics.

## Architecture

At the heart of the schema is the `OmegaEventEnvelope`, providing a zero-allocation contract via these foundational columns:
- `OmegaEventSemantics`: Strict business terms (`Side`, `Quantity`, `Price`, `NormalizedState`).
- `OmegaEventIdentity`: Safe bounds identifying `Account`, `Venue`, `Route`, and `Counterparty` routing limits.
- `OmegaEventProvenance`: Cryptographic hashes tying the parsed data back to raw evidence arrays for replay safety.
- `OmegaEventTime`: Abstract timing fields prioritizing matching-engine sequencing down to nanoseconds.
- `OmegaEventExtensions`: Strict lifetime-checked containers allowing unmapped data to safely bypass the schema without modifying the underlying structs.

## The Omega to Annex Boundary

Omega parses data from the native wire shape into an `OmegaEventEnvelope`. It then bounds validations across all 12+ integrations, proving the data against Omega's generic schema. Crucially, the generic `test_cross_venue_validation.cpp` guarantees that all mapped streams—from LSE Equities to GS routing tags—will evaluate symmetrically into the generic **QuanuX-Annex** routing projections without writing branch logic.

Annex enforces Dead-Letter Routing (`ParseStatus::Error` into `.invalid.` streams) and normalizes workflow streams, shielding the remaining AI and accounting layers from venue-specific volatility.

## Integrating New Venues and Counterparties

Expanding Omega to cover new exchanges (like EUREX) or counterparties (like Morgan Stanley) requires following a strict, zero-allocation protocol:

### 1. Build Mock Ingress Types
- Inside `include/quanux/omega/adapters/<venue_name>/`, create `_structs.hpp`. Define the native C-struct that encapsulates the required execution payload (e.g. SBE/FIX binary mappings).

### 2. Implement Semantic Translation
- Build `_adapter.hpp`. Extract data natively out of the binary mappings using immediate pointer checks or simple casting.
- Never use iterating dictionaries or nested trees—parse in `O(1)` runtime.

### 3. Maintain Memory Safety (String Views)
- **Strings must be captured permanently.** Venue identifiers or opaque routing fields must be buffered using explicitly provided fields, such as `out_envelope.extensions._backing_opaque_extension_blob` and `out_envelope.identity._backing_counterparty_id`.
- Never rely on `static char[]` buffers or unsafe pointers matching the network packet lifecycle, as that breaks concurrency boundaries.

### 4. Prove Annex Genericity
- Define a `SourceCapabilityProfile` accurately displaying the integration constraints.
- Emulate the venue lifecycle inside `tests/test_<venue>_adapter_conformance.cpp`. Ensure your adapter fails safely for malformed payloads.
- Inject your new adapter into `omega/tests/test_cross_venue_validation.cpp`. Your profile must compile natively against Annex’s strict validation logic without requiring customized NATS consumer logic mapping.

## Documentation Reference
Specific architectural decisions matching individual venues are documented directly inside each `adapters/<venue>/SKILL.md` footprint. Read these context flags before modifying integration endpoints to avoid disrupting the carefully orchestrated identity paths distinguishing venues (matching engines) from counterparties (routing pathways).
