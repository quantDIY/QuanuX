# QuanuX GCP Integration Implementation Plan (Revised)

This document outlines the execution mandate for the initial Tract 1 rollout of the QuanuX GCP Integration, adhering strictly to the `$QUANUX_HOME/gcp_architecture_approved.md` limits and incorporating Red Team structural review feedback.

## Goal Description
Implement the primary asynchronous data ingestion pipeline for the QuanuX 5-Tier topology into Google Cloud Platform (GCP). The scope is strictly bounded to the historical data lake, the Python-first modeling tier, and telemetry storage, explicitly avoiding any interaction with the Tier 4 deterministic execution path.

## Proposed Changes

### Data Ingestion Backbone (Python)
The pure Python async ingestion module serving as the bridge to GCP. To align with the existing `QuanuX-Annex` layout, we will place the new ingestion scripts at the root level of Annex alongside `quanux_critic.py` and `quanux_vault.py`.

#### [NEW] `QuanuX-Annex/gcp_ingestion_pipeline.py`
- Implements a NATS JetStream subscriber extracting the canonical `quanux.schema.MarketTick` definition.
- Uses `pyarrow` to build columnar batches strictly up to a configurable memory ceiling.
- Handles backpressure dynamically when the memory ceiling is reached by temporarily pausing JetStream consumption.
- Flushes the batched Arrow tables into Parquet format and triggers an asynchronous upload to Google Cloud Storage (GCS).

#### [NEW] `QuanuX-Annex/gcp_bigquery_setup.py`
- Registers BigQuery External Tables against the GCS Parquet bucket paths to expose the historical query surface to the modeling tier.

---

### Operator Symmetry (quanuxctl CLI)
Extending the existing `infra` command group into a provider-aware operator surface so GCP workflows follow the same operator pattern as DigitalOcean while isolating provider-specific implementation beneath the abstraction.

#### [MODIFY] `server/cli/src/quanuxctl/commands/infra_commands.py`
- Extend the `infra` Typer command group to support provider discrimination via `--provider` for GCP.
- Add `quanuxctl infra ingest-start --provider gcp --memory-limit-mb 500`
- Add `quanuxctl infra table-register --provider gcp`
- Add `quanuxctl infra nodes --provider gcp` (or modify existing nodes listing logic to accept provider discriminators).

## Verification Plan

### Automated Tests
- Create `tests/test_gcp_ingestion.py` in the repository root test suite.
- Mock the NATS JetStream layer with high-throughput dummy `MarketTick` events.
- Assert that the `pyarrow` batch sizes never exceed the tested memory parameters (verifying the Bounding Doctrine).
- Mock GCS and BigQuery APIs to validate the asynchronous upload and external table registration mechanisms.

### Manual Verification
- Start a local JetStream container (`./scripts/start_stack.sh`).
- Run `quanuxctl infra ingest-start --provider gcp --memory-limit-mb 500`.
- Inject mock traffic.
- Use `top` or a memory profiler to visually confirm the Python process heap usage remains cleanly bounded under the 500MB specified limit over prolonged execution.
