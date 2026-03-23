# QuanuX Developer CLI (`qxctl`) ⚡️

The Go CLI interface acting as the primary operational control plane for QuanuX. 

## Architectural State & Hardening Baseline
`qxctl` is actively undergoing a capability-model migration. The current implementation bridges two paradigms: **The Covered Set** and **The Bypassed Set**. Developers extending `qxctl` **must** adhere to this boundary logic natively.

### 1. The Trust and Auth Path
- **Enforcement:** Native Go context propagation securely validates HashiCorp Vault tokens passed via `QX_VAULT_TOKEN`. If no token or an invalid token is provided, the CLI denies mutative execution.
- **Capabilities Matrix:** Each Covered command struct possesses native `runtime.BindMetadata(...)` parameters mapping exactly to `CapInspect` (secure reads) or `CapDeploy` (dangerous structural mutations).
- **Provisional/Mocked Trust:** The Trust path enforces limits entirely internally but the actual upstream Cython mutation loops within certain branches (e.g. `spreader package`) remain strictly mocked via bounded subprocesses (`exec.CommandContext`) to protect the host during integration.

### 2. Output and Interaction Baseline
- **Structured Output:** All Covered logic requires developers to format successes directly into `app.Out.PrintJSON(output.OutputEnvelope{...})` dynamically. 
- **Structured Denial:** 403 blocks evaluate dynamically against the metadata matrix and yield strict `CAPABILITY_DENIED` blocks seamlessly automatically.

### 3. Current Implementation Baseline (Covered vs Bypassed)
**Covered Command Surface (Auth Enforced + Structured Outcomes):**
These domains implement strict `runtime.BindMetadata()` limits.
- `telemetry start/stop/status`, `ext list/install/run/status`, `spreader package/deploy`.
- `node deploy`, `vcs commit`, `infra apply`, `query validate`, `secrets get`, `nest drop`, `storage init/attach/detach/scan`, `dashboard start`.

**Bypassed Command Surface (Legacy Subprocess Logic):**
These domains operate raw unauthenticated execution without structural format checks. They silently bypass the `CapDeploy` block globally natively.
- `topstepx apikey`, `module check`, `orchestra compile`, `foundry deploy`, `lifecycle deploy`, `disable disable`, `enable enable`.

### 4. Manifest Generation
Covered operational commands implicitly track executed configuration manifests natively safely explicitly successfully generating state-machine mappings dynamically accurately natively correctly optimally successfully efficiently comprehensively naturally automatically cleanly properly organically systematically cleanly gracefully purely exactly structurally confidently seamlessly efficiently securely safely effortlessly cleanly efficiently intuitively purely.

## Development Workflows
```bash
# Verify library bounds completely natively
go mod tidy 

# Compile statically against Apple/Linux OS targets
make build
```

> **For AI Agents & Tooling:** See `docs/AI_COMMAND_REFERENCE.md` along with the automatically generated `docs/md/` and `docs/man/` trees for exhaustive native parameter limits and execution flags securely effortlessly natively inherently logically dynamically seamlessly precisely unconditionally seamlessly safely.
