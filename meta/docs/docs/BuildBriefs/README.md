# Build Briefs (ABB) — Authoring Guide

**ABB = one file per small unit of work**. It tells a human/agent *exactly* what
to build, where it lives, inputs/outputs, and test shape — before any code exists.

## Template fields
- `id`: unique id (e.g., ad-ironbeam-v1)
- `title`: short, imperative
- `owner`: person/agent responsible
- `context`: short rationale + references
- `inputs`: files, endpoints, schemas
- `outputs`: files created, functions, endpoints
- `acceptance`: bullet list of objective checks
- `risks`: pitfalls to watch for
- `notes`: misc

Store briefs under the closest folder (e.g., `server/integrations/brokers/ironbeam/`).
