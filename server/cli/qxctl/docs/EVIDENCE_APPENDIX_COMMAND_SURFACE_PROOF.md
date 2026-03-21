# Evidence Appendix: Command Surface Proof

This document provides explicit code-level evidence for the QuanuX Control Plane (`qxctl`) command boundaries.

## 1. Root Registration Proof

**[VERIFIED]** Exact Count: 34 Root-Level Constructors.
**File:** `server/cli/qxctl/cmd/root.go` (Lines 27-62)

```go
	rootCmd.AddCommand(
		NewBridgeCmd(app),
		NewConfigCmd(app),
		NewCrucibleCmd(app),
		NewDashboardCmd(app),
		NewDeployCmd(app),
		NewDisableCmd(app),
		NewEnableCmd(app),
		NewEngineCmd(app),
		NewExtCmd(app),
		NewFoundryCmd(app),
		NewHabitatCmd(app),
		NewIndicatorsCmd(app),
		NewInfraCmd(app),
		NewInstallCmd(app),
		NewLifecycleCmd(app),
		NewModuleCmd(app),
		NewNestCmd(app),
		NewNodeCmd(app),
		NewOrchestraCmd(app),
		NewPredeployCmd(app),
		NewProbeCmd(app),
		NewQueryCmd(app),
		NewRemoveCmd(app),
		NewSecretsCmd(app),
		NewSkillsCmd(app),
		NewSpreaderCmd(app),
		NewStorageCmd(app),
		NewTelemetryCmd(app),
		NewTopstepxCmd(app),
		NewUpgradeCmd(app),
		NewUpgradeableCmd(app),
		NewVaultCmd(app),
		NewVcsCmd(app),
		NewVersionCmd(app),
	)
```

## 2. Command Family Proof Table

*Sample of core structural families proven via constructor signatures.*

| Family | Source File | Constructor | Proof Excerpt | Child Count | Confidence |
|---|---|---|---|---|---|
| `engine` | `cmd/engine.go` | `NewEngineCmd` | `cmd.AddCommand(setupTopologyCmd, startCmd, tuneCmd)` | 3 | [VERIFIED] |
| `infra` | `cmd/infra.go` | `NewInfraCmd` | `cmd.AddCommand(applyCmd, authCmd, destroyCmd, gcpSqlCmd, ingestStartCmd, setSpacesKeysCmd, setTokenCmd, statusCmd, tableRegisterCmd)` | 9 (+3 nested) | [VERIFIED] |
| `secrets` | `cmd/secrets.go` | `NewSecretsCmd` | `cmd.AddCommand(getCmd, listCmd, setCmd, setupCmd)` | 4 | [VERIFIED] |
| `telemetry` | `cmd/telemetry.go` | `NewTelemetryCmd` | `cmd.AddCommand(restartCmd, startCmd, statusCmd, stopCmd)` | 4 | [VERIFIED] |
| `query` | `cmd/query.go` | `NewQueryCmd` | `cmd.AddCommand(estimateCmd, validateCmd)` | 2 | [VERIFIED] |

## 3. Leaf Command Proof Table (15 Analyzed Paths)

This matrix proves precisely which commands execute logic vs dummy stubs returning `nil`.

| Full Command Path | Source File | Exact Proven Excerpt | Leaf Proof | Confidence |
|---|---|---|---|---|
| `engine start` | `cmd/engine.go` | `RunE: func(cmd *cobra.Command, args []string) error { return engine.Start(app.Ctx, args[0]) }` | Leaf | [VERIFIED] |
| `engine setup-topology`| `cmd/engine.go` | `RunE: func(cmd *cobra.Command, args []string) error { return engine.SetupTopology(app.Ctx) }` | Leaf | [VERIFIED] |
| `engine tune` | `cmd/engine.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nil }` | Stub Leaf| [VERIFIED] |
| `secrets get` | `cmd/secrets.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nil }` | Stub Leaf| [VERIFIED] |
| `secrets set` | `cmd/secrets.go` | `RunE: func(cmd *cobra.Command, args []string) error { return secrets.Set(app.Ctx, args[0], args[1]) }` | Leaf | [VERIFIED] |
| `secrets setup` | `cmd/secrets.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nil }` | Stub Leaf| [VERIFIED] |
| `spreader package` | `cmd/spreader.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nil }` | Stub Leaf| [VERIFIED] |
| `spreader deploy` | `cmd/spreader.go` | `RunE: func(cmd *cobra.Command, args []string) error { return spreader.Deploy(app.Ctx, args[0]) }` | Leaf | [VERIFIED] |
| `telemetry start` | `cmd/telemetry.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nil }` | Stub Leaf| [VERIFIED] |
| `node deploy` | `cmd/node.go` | `RunE: func(cmd *cobra.Command, args []string) error { return node.Deploy(app.Ctx, args[0], hub, token, dryRun) }` | Leaf | [VERIFIED] |
| `nest drop` | `cmd/nest.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nest.Drop(app.Ctx, args[0], engine) }` | Leaf | [VERIFIED] |
| `infra apply` | `cmd/infra.go` | `RunE: func(cmd *cobra.Command, args []string) error { return infra.Apply(app.Ctx, target) }` | Leaf | [VERIFIED] |
| `infra gcp-sql execute`| `cmd/infra.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nil }` | Stub Leaf| [VERIFIED] |
| `query validate` | `cmd/query.go` | `RunE: func(cmd *cobra.Command, args []string) error { return nil }` | Stub Leaf| [VERIFIED] |
| `vault status` | `cmd/vault.go` | `RunE: func(cmd *cobra.Command, args []string) error { return vault.Status(app.Ctx, target) }` | Leaf | [VERIFIED] |

## 4. Dump Tree Evidence

**[STRONG INFERENCE]** Previously asserted the existence of precisely 58 subcommands.
**Constraint Downgrade:** The exact dump file `server/cli/qxctl/dump_tree_test.go` was created asynchronously and deleted after execution. Because it is no longer physically persistent in the `quanux-qxctl` branch trace, the output count (58) strictly downgrades from `VERIFIED` to `STRONG INFERENCE`.

**Historical dump excerpt (non-persistent):**
```text
CMD: qxctl infra apply | PURPOSE: Runs Terraform Apply strictly for the designated deployment
CMD: qxctl infra auth | PURPOSE: Outputs export commands for QECD Phase 3 Terraform Provisioning
CMD: qxctl node deploy | PURPOSE: Deploy the QuanuX Execution Node to a remote server via SSH | FLAG: --dry-run
```

## 5. Flag Inheritance Proof

**[VERIFIED]**
**File:** `server/cli/qxctl/cmd/root.go`
```go
rootCmd.PersistentFlags().String("output", "text", "Set the output UX mode (text, json, yaml, quiet)")
rootCmd.PersistentFlags().String("config", "", "Path to global config override")
```
- `[VERIFIED]` `--output` and `--config` are defined as root persistent flags and are inherited by Cobra subcommands at the flag layer.
- **Per-Command Local Bounds:** `--dry-run`, `--target`, and `--json` are locally scoped inside their respective structs (e.g. `gcpSqlExecuteCmd.Flags().Bool("dry-run", false)`).

## 6. Final Command-Surface Confidence Statement

- **Root-Family Count:** 34 (`[VERIFIED]`)
- **Leaf Command Count:** 58 (`[STRONG INFERENCE]`)
- `[VERIFIED]` 7 of the 15 explicitly analyzed command paths in Section 3 are stub leaves whose RunE bodies return nil.
- **Unresolved Ambiguities:**
  - The 58 leaf-command count remains `[STRONG INFERENCE]`.
  - The root-family count is only `[VERIFIED]` because the full registration block is embedded.
  - Only a representative subset of command paths is quoted in full.
  - Unquoted command families remain outside the direct excerpt set.
