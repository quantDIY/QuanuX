---
description: AI Context Manual for QuanuX-Omega Goldman Sachs Counterparty Module
---

# `goldman_sachs` Agent Skill

## 1. Subdirectory Context & Purpose
This directory (`counterparties/goldman_sachs/`) encapsulates the execution connectivity and parsing abstraction for Goldman Sachs as a routing counterparty. Deployed during Phase 11, it establishes that Omega distinguishes strictly between *where* an execution happened (Venue) and *who* handled the route (Counterparty).

1. **Pre-Trade Map:** Links to connectivity handlers enabling algorithmic DMA or sponsored access boundaries.
2. **Post-Trade Decoder:** Converts street-side execution drops into the universal `OmegaEventEnvelope` identically to direct venue integrations.

## 2. Intentional Engineering
- **Counterparty Identity Isolation**: It binds string constraints (`GS`) exclusively to `out_envelope.identity.counterparty_id` instead of hijacking `venue_id`.
- **Explicit Route Identifiers**: Captures dynamic routing paths mapping them to `route_id`.
- **Generic Equivalence**: Extensively proven to clear the same abstract validation checks inside Annex as a hard DMA venue (like CME).

## 3. Rules of Interaction
1. **Never Blur Venue and Counterparty**: `venue_id` must remain vacant unless explicitly parsed off the street drop as executed exchange data, distinct from GS identity.
2. **Maintain Compatibility**: Changes must continue routing into NATS JetStream equivalently to standard venue flows.
