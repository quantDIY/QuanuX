# QuanuX Roadmap (v0.0.1)

> Draft scaffolding roadmap — evolves as wiring begins.

## Phase 0 — Scaffolding (now)
- ✅ Repo hygiene, Suite/Server/Client manifests
- ✅ CLI skeleton (`quanuxctl`)
- ✅ Security stubs (auth façade, secrets broker, outbound policies)
- ✅ AI provider registry (manifests)
- ✅ DevEx env (Anaconda-first, pnpm workspace)
- ✅ CI scaffold (non-blocking)

## Phase 1 — Wiring Foundations
- Server runtime bootstrap (FastAPI/WS stubs, event bus adapter)
- Strategy engine surface + job scheduler hooks
- Data Lake I/O paths (DuckDB local, MotherDuck optional)
- Observability plumb (OTLP, error taxonomy)
- Client shell (Tauri/React) reads health + sample streams

## Phase 2 — Adapters MVP
- Broker: (pick one) **Ironbeam** or **Alpaca** adapter MVP
- Data feed: **Databento** historical + live scaffold
- Charting: **tradingview** connector surface
- OpenAPI mapping + idempotency keys

## Phase 3 — AI Co-Dev
- Agents “read the repo”: docs/context indexing, prompts
- Safe codegen lanes with Build Briefs + tests harness
- MCP/tooling alignment

## Phase 4 — Packaging
- Suite installers per OS target; signing (Cosign)
- Store submissions (macOS/Windows), Linux (deb/snap/flatpak/appimage)

> Time is tracked in GitHub Projects; this file stays strategic, not granular.
