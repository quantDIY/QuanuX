#!/usr/bin/env python3
import os, json, textwrap
from datetime import datetime, timezone

NOW = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
ROOT = os.path.abspath(".")

def ensure(p): os.makedirs(p, exist_ok=True)
def write(path, content):
    ensure(os.path.dirname(path))
    with open(path, "w", encoding="utf-8") as f: f.write(content)

# Root meta
write(f"{ROOT}/README.md", f"""# QuanuX — Full Context Snapshot (Docs & Scaffolds Only)
Generated: {NOW}

This snapshot contains the architecture decisions, policies, schemas, adapter scaffolds, and
integration notes we discussed. It is not runtime code yet—intended to seed wiring work.
""")
write(f"{ROOT}/.gitignore", ".DS_Store\n__pycache__/\n*.pyc\n.env*\n/build\n/dist\n*.log\n")

# 1) Core docs + context
write(f"{ROOT}/docs/architecture/decision_board_v1.md", textwrap.dedent(f"""
    # QuanuX Decision Board — v1
    Generated: {NOW}
    Status: **DECIDED**, additive-only until v0.1.

    - Event bus: NATS JetStream (streams); Redis cache-only
    - Workflows: Temporal (feature-flagged)
    - Data lake: DBN + Parquet; DuckDB local; MotherDuck optional; S3/MinIO archive
    - Security: Vault/Keychain; mTLS; egress allowlist per adapter
    - OpenAPI v0.0.1 frozen; vendor flags in per-adapter mapping.yaml
    - Observability: OTLP + Prom/Grafana; shared error taxonomy (20–30 codes)
    - Release: Sigstore Cosign; distroless; pinned digests
    - Time: UTC internal; user-confirm at first run (display only)
    - DevEx: Monorepo (pnpm + poetry/uv + conda); Ruff/Black/mypy; Biome/ESLint/Prettier
"""))
write(f"{ROOT}/docs/architecture/decision_board_v1.json", json.dumps({
    "generated_at": NOW,
    "decisions": [
        {"id":"A1","decision":"NATS JetStream for event streaming; Redis cache-only."},
        {"id":"A2","decision":"Temporal for workflows (feature-flagged)."},
        {"id":"B1","decision":"DBN + Parquet; DuckDB local; optional MotherDuck mirror."},
        {"id":"C1","decision":"Provenance headers opt-in OFF by default."},
        {"id":"C2","decision":"mTLS server↔server/bridge; optional client↔server."},
        {"id":"C3","decision":"Secrets: Vault/Keychain; Redis not for persistence."},
        {"id":"D1","decision":"OpenAPI v0.0.1 frozen; additive-only."},
        {"id":"E1","decision":"Shared error taxonomy mapped by adapters."}
    ]
}, indent=2))
write(f"{ROOT}/docs/architecture/scrum_review.md", "# SCRUM Review\n- Maintain separation of concerns\n- Adapter boundaries enforced\n- Agent-friendly docs\n")
write(f"{ROOT}/docs/Config/precedence.md", "# Configuration Precedence\nDefaults → YAML → ENV → CLI → VAULTREF.\n")
write(f"{ROOT}/docs/Contrib/style.md", "Python: Ruff/Black/mypy. TS: Biome/ESLint + Prettier. Conventional Commits.\n")
write(f"{ROOT}/docs/Repo/policy.md", "Monorepo with pnpm + poetry/uv + conda; adapters as plugins; no cross-imports.\n")
write(f"{ROOT}/context/QuanuX-Context-Bootstrap.md", "# Context Bootstrap\nPaste into new chat to seed assistant with decisions & policies.\n")
write(f"{ROOT}/context/AI-Kickoff-Prompt.txt", "Honor Decision Board v1. Adapters via entry points. OpenAPI v0.0.1 frozen.\n")
write(f"{ROOT}/context/README.md", "Use AI-Kickoff-Prompt to seed a new conversation.\n")
write(f"{ROOT}/config/timezone.json", json.dumps({"mode":"prompt","effective_timezone":"UTC","generated_at":NOW}, indent=2))

# 2) Security + Observability
write(f"{ROOT}/server/security/outbound_policies.yaml", "default: deny\nadapters: {}\n")
write(f"{ROOT}/docs/Security/AuthFlows.md", "mTLS server↔server/bridge; optional client mTLS; OAuth/OIDC for cloud adapters; provenance opt-in.\n")
write(f"{ROOT}/docs/Security/mTLS.md", "mTLS across services; rotate certs every 90 days; store private keys in secure keystore.\n")
write(f"{ROOT}/docs/Security/Secrets.md", "Use Vault or OS Keychain; never YAML; refer as vaultref://path/to/secret\n")
write(f"{ROOT}/server/observability/error_taxonomy.yml", "errors:\n  - code: E_INTERNAL\n    message: Unexpected internal error\n    severity: CRITICAL\n  - code: E_VENDOR\n    message: Upstream vendor error\n    severity: ERROR\n  - code: E_RATE_LIMIT\n    message: Rate limit exceeded\n    severity: WARN\n")
write(f"{ROOT}/docs/SLAs/initial.md", "# Initial SLOs\n- Orders p95 ≤ 200ms; reconnect < 3s; replay ≥ 50k/s.\n")
write(f"{ROOT}/server/observability/otel.md", "OTLP traces/logs/metrics; Prom/Grafana dashboards; trace IDs end-to-end.\n")

# 3) Integrations + Runtime + Data
write(f"{ROOT}/server/integrations/vendor_capabilities_rate_limits.patch.yml", "vendors:\n  _example:\n    rate_limits:\n      rpm: TBD\n      burst: TBD\n")
write(f"{ROOT}/server/integrations/_templates/README.md", "# Adapter Template\n- Implement hooks\n- Provide mapping.yaml\n- Keep flags vendor-local\n")
write(f"{ROOT}/server/integrations/_templates/hooks.py", "class AdapterHooks:\n    def on_start(self):\n        pass\n    def on_stop(self):\n        pass\n")
write(f"{ROOT}/server/runtime/feature.flags.yml", "temporal:false\nmtls_required:true\n")
write(f"{ROOT}/server/runtime/jobs.md", "Temporal workflows, backfills, replays; feature-flagged.\n")
write(f"{ROOT}/server/runtime/event_bus.md", "NATS JetStream subjects: orders.*, market.*, audit.*\n")
write(f"{ROOT}/server/strategies/runtime/README.md", "Modes: in-process, container, k8s. Health endpoints: /healthz, /readyz, /metrics.\n")
write(f"{ROOT}/docs/DataLake/layout.md", "DBN + Parquet; DuckDB local; MotherDuck optional; S3/MinIO archive.\n")
write(f"{ROOT}/docs/DataLake/retention.md", "30d hot retention; archive older partitions.\n")
write(f"{ROOT}/docs/API/idempotency.md", "Idempotency-Key required on mutations; 409 on duplicate.\n")

# JSON Schemas
write(f"{ROOT}/schemas/json/events/envelope.schema.json", json.dumps({
  "$schema":"https://json-schema.org/draft/2020-12/schema",
  "title":"Event Envelope",
  "type":"object",
  "required":["event_type","sequence","correlation_id","source","timestamp","payload"],
  "properties":{
    "event_type":{"type":"string"},
    "sequence":{"type":"integer","minimum":0},
    "correlation_id":{"type":"string"},
    "source":{"type":"string"},
    "timestamp":{"type":"string","format":"date-time"},
    "payload":{"type":"object"}
  },
  "additionalProperties": False
}, indent=2))
write(f"{ROOT}/schemas/json/trading/order_request.schema.json", json.dumps({
  "$schema":"https://json-schema.org/draft/2020-12/schema",
  "title":"Order Request",
  "type":"object",
  "required":["symbol","side","quantity","type","time_in_force","client_order_id"],
  "properties":{
    "symbol":{"type":"string"},
    "exchange":{"type":["string","null"]},
    "side":{"type":"string","enum":["BUY","SELL"]},
    "type":{"type":"string","enum":["MARKET","LIMIT","STOP","STOP_LIMIT"]},
    "price":{"type":["number","null"]},
    "stop_price":{"type":["number","null"]},
    "time_in_force":{"type":"string","enum":["DAY","GTC","IOC","FOK"]},
    "quantity":{"type":"number","minimum":0},
    "client_order_id":{"type":"string"},
    "flags":{"type":"object","description":"vendor-specific flags (adapter-local)"},
    "idempotency_key":{"type":"string"},
    "timestamp":{"type":"string","format":"date-time"}
  },
  "additionalProperties": False
}, indent=2))
write(f"{ROOT}/schemas/json/trading/precision.schema.json", json.dumps({
  "$schema":"https://json-schema.org/draft/2020-12/schema",
  "title":"Instrument Precision",
  "type":"object",
  "required":["symbol","price_tick","quantity_step"],
  "properties":{
    "symbol":{"type":"string"},
    "price_tick":{"type":"number","exclusiveMinimum":0},
    "quantity_step":{"type":"number","exclusiveMinimum":0},
    "multiplier":{"type":["number","null"]},
    "decimals":{"type":["integer","null"]}
  },
  "additionalProperties": False
}, indent=2))
write(f"{ROOT}/schemas/json/common/error.schema.json", json.dumps({
  "$schema":"https://json-schema.org/draft/2020-12/schema",
  "title":"QuanuX Error",
  "type":"object",
  "required":["code","message","correlation_id"],
  "properties":{
    "code":{"type":"string"},
    "message":{"type":"string"},
    "correlation_id":{"type":"string"},
    "details":{"type":["object","null"]}
  },
  "additionalProperties": False
}, indent=2))

# 4) Platform + AI + Targets
write(f"{ROOT}/docs/AI/manifest.yaml", "training_sources:\n  - docs/architecture\n  - docs/Config\n  - docs/Security\n  - docs/DataLake\n  - docs/API\n")
write(f"{ROOT}/docs/BuildBriefs/README.md", "# Architecture Build Briefs (ABBs)\nPurpose, schema and examples to guide agents.\n")
write(f"{ROOT}/docs/BuildBriefs/template.abb.md", textwrap.dedent("""\
    ---
    type: abb
    id: TEMPLATE-ABB
    title: Example ABB
    status: draft
    ---

    ## Purpose
    Explain the objective and constraints.

    ## Inputs
    List inputs/artifacts.

    ## Outputs
    List files and acceptance checks.
"""))
write(f"{ROOT}/containers/BASELINES.md", "Base images: distroless-lean (services), debian-slim (tools); pinned digests; CUDA profiles.\n")
write(f"{ROOT}/docs/Dist/Matrix.md", "Targets: Ubuntu/macOS/Win11; Debian/Alma/Amazon/Arch; DGX OS (x86_64/ARM64); Tauri client packaging.\n")
write(f"{ROOT}/docs/Release/signing.md", "Sign artifacts with Cosign; publish checksums; verify offline.\n")
write(f"{ROOT}/docs/Time/time.md", "User-confirmed UTC; store internal UTC; display local optionally.\n")
write(f"{ROOT}/docs/TradingView/integration_notes.md", "Use 'tradingview' naming; SSO optional; data privacy caveats.\n")
write(f"{ROOT}/docs/Targets/DGX_OS_profiles.md", "DGX OS (x86_64 & ARM64): CUDA/NCCL/toolkit notes; container runtime.\n")
write(f"{ROOT}/docs/Targets/Linux_Server_Matrix.md", "Ubuntu/Debian/Alma/Amazon/Arch profiles; package formats (deb/rpm/snap/flatpak/appimage).\n")

print(f"✅ Wrote enriched QuanuX docs into: {ROOT}")
