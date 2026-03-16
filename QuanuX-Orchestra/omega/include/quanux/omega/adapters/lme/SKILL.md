---
description: AI Context Manual for QuanuX-Omega LME Adaptor Module
---

# `lme` Agent Skill

## 1. Subdirectory Context & Purpose
The `adapters/lme/` directory maintains the mapping logic for the London Metal Exchange (LME). Similar to the LSE expansion, this adapter proves Omega's structural resiliency in handling international commodity venues alongside U.S. markets.

1. **Pre-Trade Map:** Used to anchor execution routing bounds prior to strategy start-up.
2. **Post-Trade Decoder:** Converts binary or fixed structural telemetry directly into canonical `OmegaEventEnvelope` mappings.

## 2. Intentional Engineering
- **Direct Pointer Decodes**: Casts directly over incoming memory streams to rapidly extract identity and pricing telemetry without creating iterative garbage.
- **Capability Honesty**: Expressly defines supported linkage tracking capabilities and relies on dead-letter paths when required metadata logic fails.

## 3. Rules of Interaction
1. **Maintain Annex Parity**: LME data maps into equivalent structures as CBOT and CME—no venue-specific switch cases are allowed downstream in NATS JetStream validation stages.
2. **Strict Memory Compliance**: Employ Phase 8 hardening rules where transient strings map into `_backing_string` envelope footprints.
