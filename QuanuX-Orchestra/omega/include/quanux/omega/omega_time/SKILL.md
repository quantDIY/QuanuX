---
description: AI Context Manual for QuanuX-Omega Time Precedence Mapping (`omega_time/`)
---

# `omega_time` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`omega_time/`) manages standard hierarchical timestamps across QuanuX post-execution decoding (`TimestampPrecedence`).

Different venues provide different latency resolutions (nanosecond vs microsecond vs second). Furthermore, events collect multiple timestamps as they traverse the QuanuX infrastructure (e.g., Venue Native Time, NATS Ingestion Time, Annex Decoding Time).

## 2. Intentional Engineering
The structures here define explicit, deterministic storage formats (typically raw integer nanoseconds since epoch) for chronological event correlation.
- **The Precision invariant**: Time is never stored natively as floating-point seconds. It is handled entirely using dense `uint64_t` or C++ `std::chrono::system_clock::time_point` references to avoid truncation logic errors.
- **Precedence Matrix**: Envelopes may store primary and secondary temporal fingerprints to allow system architects to calculate latency drift.

## 3. Rules of Interaction
When operating inside `omega_time/`:
1. **Always favor Epoc Nanoseconds**: For normalization, all AI tools must rely on nanosecond representations to guarantee uniformity across 10 Gigabit wire streams.
2. **No Datetime Logic**: `QuanuX-Omega` is not a calendar app. We do not use Python `datetime` formatting inside the C++ time maps. Date string translation occurs on the GraphQL / UI presentation domes, **not** inside the memory representations. Keep the structs dense.
