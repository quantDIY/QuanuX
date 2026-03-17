# QuanuX Ansible Configuration

This directory manages the configuration and state of the QuanuX infrastructure network.

## Deployment Profiles

**DigitalOcean (Legacy/Standard):**
General purpose setup using `01-mesh-bootstrap.yml`, `02-panopticon-observability.yml`, etc.
Target resolution automatically falls back to `QUANUX_TARGET=do`.

**Google Cloud Platform (QECD Phase 4):**
Dedicated highly performant edge tuning via `06-gcp-engine-tune.yml` and `07-gcp-engine-start.yml`.
Target resolution uses `QUANUX_TARGET=gcp`.
GCP nodes (c2-standard) rely on specific kernel physics overrides for C++ deterministic execution.

## Execution
Use `quanuxctl engine [tune|start] --target [do|gcp]` rather than running `ansible-playbook` manually to guarantee environment symmetry.
