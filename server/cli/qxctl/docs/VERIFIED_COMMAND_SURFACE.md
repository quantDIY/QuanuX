# Verified Command Surface

This document explicitly catalogs the exact `qxctl` command surface dynamically scraped from the native Go binary (`quanux-qxctl` branch). 

### 1. Structural Constants
- **[VERIFIED] Constructor File Count**: 34 (`server/cli/qxctl/cmd/*.go` bounded by `COMMAND_CENSUS.md`).
- **[VERIFIED] Leaf-Command Count**: 58 (Mapped natively from traversing `rootCmd.Commands()`).

### 2. Verified Leaf Command Enumeration

*(Sample of critical paths proven from `dump_tree_test.go` output stream)*

| Full Command Path | Parent Cmd | Source File | Constructor | Leaf? | Output Flag? | Dry-Run Flag? | Interactive? | Confidence |
|---|---|---|---|---|---|---|---|---|
| `qxctl infra apply` | `infra` | `cmd/infra.go` | `NewInfraCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl infra auth` | `infra` | `cmd/infra.go` | `NewInfraCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl infra destroy` | `infra` | `cmd/infra.go` | `NewInfraCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl infra gcp-sql execute` | `infra gcp-sql` | `cmd/infra.go` | `NewInfraCmd(app)` | Yes | `--json` explicitly mapped | **Yes** | No | [VERIFIED] |
| `qxctl engine start` | `engine` | `cmd/engine.go` | `NewEngineCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl engine setup-topology`| `engine` | `cmd/engine.go` | `NewEngineCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl node deploy` | `node` | `cmd/node.go` | `NewNodeCmd(app)` | Yes | `--output` mapped natively | **Yes** | No | [VERIFIED] |
| `qxctl nest drop` | `nest` | `cmd/nest.go` | `NewNestCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl secrets setup` | `secrets` | `cmd/secrets.go` | `NewSecretsCmd(app)` | Yes | `--output` mapped natively | **No** | **Yes** | [VERIFIED] |
| `qxctl secrets get` | `secrets` | `cmd/secrets.go` | `NewSecretsCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl spreader package`| `spreader` | `cmd/spreader.go`| `NewSpreaderCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl spreader deploy` | `spreader` | `cmd/spreader.go`| `NewSpreaderCmd(app)` | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl query validate`  | `query`    | `cmd/query.go`   | `NewQueryCmd(app)`    | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl vault status`    | `vault`    | `cmd/vault.go`   | `NewVaultCmd(app)`    | Yes | `--output` mapped natively | **No** | No | [VERIFIED] |
| `qxctl telemetry start` | `telemetry`| `cmd/telemetry.go`|`NewTelemetryCmd(app)`| Yes | `--output` mapped natively | **No** | No | [VERIFIED] |

### 3. Ambiguous Registrations

- **[VERIFIED]** Global overriding bounds (`--output`, `--config`, `--hub`, `--trace`, `--verbose`) are registered physically on `rootCmd.PersistentFlags()` inside `cmd/root.go`. Thus, every single leaf natively inherits `--output` support implicitly, though execution handlers may fail to utilize `app.Out.Print()`.
- **[WEAK INFERENCE]** Whether explicit strings emitted from inner `pkg/` interfaces correctly observe `app.Out.Mode` limits. Native Go log loops might bypass.
