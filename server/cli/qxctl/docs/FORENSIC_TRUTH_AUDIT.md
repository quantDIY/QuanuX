# Forensic Truth Audit

This document audits the execution-contract specs against the physical realities of the `quanux-qxctl` branch.

### 1. COMMAND_METADATA_REGISTRY_DRAFT.md
- **Claim**: Cobra natively supports `*cobra.Command.Annotations`.
  - **Status**: [VERIFIED] (Built-in Cobra library feature mapping).
- **Claim**: `CommandMetadata` struct natively exists in code.
  - **Status**: [PROPOSAL] (Does not exist in `internal/runtime/` today).
- **Claim**: `BindMetadata` function exists to map interfaces cleanly.
  - **Status**: [PROPOSAL].
- **Claim**: `engine start`, `secrets setup`, `query validate` are existing commands.
  - **Status**: [VERIFIED] (Verified via dynamic `dump_tree_test.go` introspection).
- **Overreach**: Asserted that test automation for metadata completeness (`TestCommandMetadataCompleteness`) was trivial, without acknowledging that Cobra `RunE` vs `Run` pointers require reflection checks defensively.
- **Survives**: The structural JSON interface approach.
- **Rewrites Needed**: None currently, as the document accurately labeled the Struct and Metadata Binding Strategy as `[PROPOSAL]`.

### 2. STRUCTURED_OUTPUT_AND_ERROR_SPEC.md
- **Claim**: Outputs use `app.Out.Print(data)` or `app.Out.EmitRaw(string)`.
  - **Status**: [VERIFIED] (`server/cli/qxctl/internal/output/ux.go`).
- **Claim**: Errors use `qxerrors.Exit(err)` dumping raw panic strings to stderr.
  - **Status**: [VERIFIED] (`server/cli/qxctl/main.go`).
- **Claim**: Exit codes are universally `1` on failure currently natively.
  - **Status**: [STRONG INFERENCE] (`qxerrors.Exit(err)` generally delegates to `os.Exit(1)` natively without custom `exitError` mappings, but sub-packages may override).
- **Claim**: `OutputEnvelope` struct natively exists.
  - **Status**: [PROPOSAL].
- **Claim**: Subprocesses like `terraform` leak raw output to `os.Stdout`.
  - **Status**: [WEAK INFERENCE] (Not proven via direct file inspection of `pkg/infra/apply.go`, inferred from shell patterns).
- **Overreach**: The JSON schema proposal implies `terraform` outputs cleanly deserialize dynamically; in truth, intercepting raw stdout from terraform plugins requires `io.Pipe` blocking reads natively.
- **Rewrites Needed**: Subprocess Wrapping Policy must account for `os.Pipe` rather than simple `bytes.Buffer`.

### 3. CAPABILITY_ENFORCEMENT_INSERTION_PLAN.md
- **Claim**: `qxctl` assumes user absolute authority natively.
  - **Status**: [VERIFIED] (No capability fields observed in `app.go` or `root.go`).
- **Claim**: `runtime.App` injects `Cfg`, `Ctx`, `Out`, `NATS`, `Vault`.
  - **Status**: [VERIFIED] (`server/cli/qxctl/internal/runtime/app.go`).
- **Claim**: `SessionToken` structure maps agent bounds natively.
  - **Status**: [PROPOSAL].
- **Claim**: `app.AuthorizeAndPrepare()` binds dynamically to Cobra `PreRunE`.
  - **Status**: [PROPOSAL].
- **Overreach**: Claimed that `dry-run` logic cleanly downgrades "deploy" rings to "simulate." This assumes physical deployment bounds don't leak `net.Dial` calls outside `RunE` blocks statically.
- **Survives**: The `PreRunE` boundary insertion vector.

### 4. MANIFEST_GENERATOR_PLAN.md
- **Claim**: No manifest schema exists natively. Help commands emit plaintext humans strings natively.
  - **Status**: [VERIFIED] (Standard Cobra `--help` implementation exists).
- **Claim**: `qxctl cluster manifest` command traverses `cmd.Root()`.
  - **Status**: [PROPOSAL].
- **Claim**: Github Action executes CI checks natively.
  - **Status**: [PROPOSAL].
- **Overreach**: N/A. The spec explicitly labels the file `cmd/ext/manifest.go` as `[PROPOSAL] / [New File]`.

### 5. EXECUTION_CONTRACT_PATCH_ORDER.md
- **Claim**: 10 patch steps map the structural refactoring natively.
  - **Status**: [PROPOSAL].
- **Overreach**: Asserted Blast Radius scores (`Low`, `Medium`) without physical boundary testing.
- **Rewrites Needed**: Prerequisite graphing must be stricter. Step 10 (CI Guard) requires Step 9 (Manifest Generator) statically.
