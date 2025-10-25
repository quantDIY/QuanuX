---
title: "QuanuX Developer Onboarding Quickstart"
version: v0.0.1
status: Draft (Scaffolding)
last_updated: 2025-10-24
---

# 🧭 Welcome to QuanuX

QuanuX is a Linux-first, cross-platform, open-source quantitative trading ecosystem built for research, data science, and algorithmic trading.
Its mission is to unify data ingestion, model training, backtesting, and live execution under a modular, AI-assisted, and developer-friendly framework.

This document introduces new developers (human and AI alike) to the QuanuX architecture, repository structure, development workflows, and contribution practices.

---

## Environment Setup

System Requirements:
- OS: Ubuntu LTS (latest two) / Debian / AlmaLinux / Amazon Linux / macOS / Windows 11 (Linux preferred)
- Python: 3.11+ (Anaconda base environment)
- Node.js: 20+ (pnpm preferred)
- Rust: stable toolchain (for Tauri)
- Docker or Podman: latest
- Redis: latest
- Git: latest
- Optional GPU: CUDA or ROCm

Install via Anaconda:
- conda create -n quanux python=3.11
- conda activate quanux
- conda install nodejs rust redis docker-compose -c conda-forge

---

## Repository Structure (Simplified)

- client/ — React + Tauri frontend (desktop/web), React Native shells (mobile/tablet/TV)
- server/ — Python runtime, adapters, data lake, observability, job scheduler
- integrations/ — brokers, data feeds, charting, AI providers, anaconda
- schemas/ — OpenAPI + JSON schemas
- docs/ — architecture, build briefs, security, targets, etc.
- containers/ — Docker/K8s templates and baselines
- context/ — AI ingestion prompts and agent instructions

---

## Running the Platform

Local Dual-Instance (default):
- Start backend server (placeholder until wiring): python3 -m quanux.server
- Start frontend client:
  - cd client
  - pnpm install
  - pnpm dev

Remote-Server Mode:
- Configure env vars:
  - QUANUX_SERVER_URL="https://yourserver.domain"
  - QUANUX_AUTH_MODE="webauthn|yubikey|oauth2"

---

## Security & Secrets

- Use Redis-backed vault or OS keyring for secrets (no plaintext).
- MFA & YubiKey supported but optional.
- Principle of least privilege.
- See docs/Security/Secrets.md and docs/Security/mTLS.md.

---

## AI Collaboration Layer

Agents can:
- Read /docs and /context to learn conventions.
- Use Build Briefs in /docs/BuildBriefs to generate safe boilerplate.
- (Planned) CLI: quanuxctl ai-train --target=runtime

Each major folder will add AI Notes explaining expected patterns.

---

## Contributing New Modules

- Branch: git checkout -b feature/<area>-<name>
- Place code & docs per structure (e.g., server/integrations/<provider>/)
- Include README.md + manifest.yaml in integrations
- Tests: pytest (server) and pnpm test (client)
- Document decisions in /docs/BuildBriefs

---

## CI/CD & DevOps

- Linting: ruff, eslint
- Type-check: mypy, tsc
- Schema validation
- Security scans
- Docs sync to DeepWiki (planned)

---

## Handy Commands (planned)

- quanuxctl start — Launch server and client locally
- quanuxctl build — Build Suite artifacts
- quanuxctl ai-train — Train AI on local context
- quanuxctl diagnose — System diagnostics + telemetry
- quanuxctl pack-docs — Rebuild docs zip bundles

---

## Developer Checklist

- [ ] Anaconda env ready; Redis/Docker installed
- [ ] .env configured securely
- [ ] Read system_overview_diagram.md
- [ ] One example adapter scaffolded
- [ ] CI checks pass locally

This doc is Draft v0.0.1 and will evolve with wiring.
