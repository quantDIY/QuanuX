
# QuanuX Decision Board — v1
Generated: 2025-10-24T15:07:52Z
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
