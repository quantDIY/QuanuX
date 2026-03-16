---
description: AI Context Manual for QuanuX-Omega JPMorgan Counterparty Module
---

# `jpmorgan` Agent Skill

## 1. Subdirectory Context & Purpose
The `counterparties/jpmorgan/` directory holds the semantic mapping boundaries for executing via J.P. Morgan paths. It builds upon Phase 11's architectural requirement to separate executing brokers from physical venues gracefully within Omega's constraints.

1. **Pre-Trade Map:** Establishes parsing targets for J.P. Morgan provided executions/FIX drops.
2. **Post-Trade Decoder:** Converts execution updates (like `'F'` for Fill) into canonical semantics while distinctly extracting string footprints into Counterparty identity layers.

## 2. Intentional Engineering
- **Venue vs. Broker Distinctions**: Extracts `JPM` into `counterparty_id`, but seamlessly maps underlying physical exchange identifiers (e.g. `XNYS` or `XNAS`) into `extensions.opaque_extension_blob` safely using string-view backups.
- **Annex Contract Integrity**: Emits canonical schemas precisely matched against the required `OmegaEventEnvelope` validations proving that venue parity applies to banking counterparties as well.

## 3. Rules of Interaction
1. **Don't Overwrite Context**: The `jpmorgan_adapter` must ensure any extracted physical venue mapping doesn't collide with JPM's identity tracking strings.
2. **Deterministic Processing**: Maintain zero allocation constraints globally. Use fixed scans to generate IDs deterministically.
