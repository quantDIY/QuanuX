---
title: "QuanuX System Overview (Text Architecture)"
version: v0.0.1
status: Draft (Scaffolding)
last_updated: 2025-10-24
---

# 🧱 QuanuX System Overview (Text Edition)

QuanuX comprises Client, Server, and Suite (packaged together). This document maps responsibilities and flows for scaffolding alignment.

---

## Core Components

- Client (Frontend): Tauri + React (desktop/web) and React Native (mobile/tablet/TV shells). Pure display/interaction; no heavy computation.
- Server (Backend): Python runtime, adapters, data lake, observability, job scheduler, event bus.
- Suite (Full Build): Bundled Client + Server for local single-machine workflows.

---

## Data Flow (Conceptual)

Market Data Sources
  │
  ▼
Integrations (Data Feed / Broker / Charting)
  │
  ▼
Adapter & Translation Layer → QuanuX OpenAPI
  │
  ▼
Event Bus + Strategy Engine
  │
  ├─→ Orders via Broker Adapters
  │
  └─→ Observability (metrics/logs/traces)
          │
          ▼
        Client UI

---

## AI Layer

- Agents consume /docs and /context to learn patterns.
- MCP alignment for external AI toolchains.
- Scoped agents per module (e.g., runtime, adapters).

---

## Data Layer

- DuckDB local lake for DBN/CSV/JSON; MotherDuck optional cloud sync.
- External drives supported; schemas under /schemas/json/...

---

## Integrations Layer

- Categories: brokers, data_feeds, charting, ai_providers, anaconda.
- Each integration ships with README.md + manifest.yaml + adapter scaffold.
- Bi-directional mapping supported where possible.

---

## Security & Auth

- WebAuthn/YubiKey, OAuth2 (as needed), mTLS for client↔server.
- Redis-backed secrets store and outbound payload signing.
- Outbound access policies: server/security/outbound_policies.yaml.

---

## Runtime & Orchestration

- Async event bus (websocket-first), job scheduler, hot-restart of services.
- Multi-threaded; GPU-aware profiles (CUDA/ROCm) for ML/backtests.
- server/runtime/feature.flags.yml for toggles.

---

## Observability

- OpenTelemetry native; error taxonomy at server/observability/error_taxonomy.yml.
- Stream logs/metrics to dashboards (Grafana/Prometheus/ELK).

---

## DevOps

- Docker/K8s/systemd deployment tracks in containers/.
- quanuxctl to control builds, diagnostics, orchestration.

---

## Versioning & Roadmap

- Semver with v0.0.x during scaffolding; widen only when wiring stabilizes.
- Roadmap markers live in docs/architecture/ and release notes.
