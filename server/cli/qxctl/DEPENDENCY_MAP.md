# QuanuX CLI Native Dependency Map (Operator-Grade)

## Overview
The QuanuX Control CLI (`qxctl`) has been meticulously restructured to entirely drop Viper globals, `init()` command mappings, and OS-bound environment leakages. Dependency injection is passed explicitly down the Cobra topology via explicit Context pointers and Lazy Execution boundaries.

## Architecture Topology

### 1. **Entrypoint (`main.go`)**
- Maps pure `context.Context` signals handling standard OS interrupts directly.
- Parses runtime arguments structurally ignoring `init()` configurations.
- Injects native `runtime.App` instantiation structurally to `cmd.NewRootCmd()`.

### 2. **Dependency Container (`internal/runtime/app.go`)**
- Encapsulates explicit dependency providers instead of exposing flat properties:
  - `Cfg *config.Config`: Strong-typed native struct.
  - `Out *output.Manager`: Redaction-equipped JSON/YAML mapping matrix.
  - `NATS providers.JetStreamEngine`: Interface-bound Engine isolating TCP dependencies.
  - `Vault providers.SecretStore`: Abstract interface isolating PKI handshakes.
  - `Ctx context.Context`: Deep propagation bound mapping `*output.Manager` inherently.

### 3. **Command Mapping (`cmd/[package].go`)**
- **ZERO `init()` declarations.** All 34 execution limbs map natively inside `func NewXCmd(app *runtime.App)` factories.
- **Lazy Evaluation**: Internal package executions (e.g., `pkg/node`, `pkg/engine`) are strictly encapsulated behind Cobra `RunE` handlers. Binary memory maps remain pristine until literal invocation boundaries trigger imports natively.

### 4. **Redaction Matrix (`internal/output/ux.go` + `internal/security/redact.go`)**
- ALL raw string buffers funnel identically through `EmitRaw` masking NATS passwords, Vault Keys, and Bearer Tokens natively preventing CI log leaks entirely.

### 5. **Plugin Host Boundary (`pkg/ext/plugin.go`)**
- Abstractly couples `hashicorp/go-plugin` directly over strict Memory Handshakes targeting purely structured Protobuf formats dynamically.
- Requires symmetrical Airgap configuration matching exactly `QUANUX_PLUGIN_SYSTEM`.

### 6. **Legal Import Targets for `cmd/*`**
Execution Limbs MUST ONLY IMPORT:
- `github.com/QuanuX/qxctl/internal/config`
- `github.com/QuanuX/qxctl/internal/output`
- `github.com/QuanuX/qxctl/internal/runtime`
- `github.com/spf13/cobra`
- Direct downstream local abstractions bound structurally to closures securely.

Any physical `pkg/` integrations MUST execute lazily through constructor execution.

## Containment Integrity Complete
By segregating the Mock mappings, Redaction logic, and Lazy Context bounds natively, the resulting binary is completely decoupled from configuration sprawl and isolated strictly inside pure memory constraints.
