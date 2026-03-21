# Tranche Two Capability Model

## 1. Capability Classes
The Tranche One metadata structs defined the following authorization bounds:
- `observe`, `inspect`, `validate`, `simulate` (Non-Mutating Evaluators)
- `deploy`, `mutate`, `destroy`, `secrets-read`, `secrets-write`, `infra-admin`, `execution-admin` (High-Blast Mutators)

## 2. Mapping Strategy
Evaluation occurs within a newly defined `runtime.App.Authorize(cmd *cobra.Command)` intercept executing prior to `RunE`.
- The `CapabilityClass` field embedded into `cmd.Annotations["qxctl_metadata"]` defines the exact requirement.
- `[PROPOSAL]` The caller's injected authorization Token enumerates held capabilities.
- A strict subset inclusion check ensures the caller possesses the target capability.

## 3. Denial Output Shape
If authorization verification fails, the intercept halts execution and returns a precise error.
**JSON Definition:**
```json
{
  "status": "error",
  "exit_code": 403,
  "error": {
    "type": "CAPABILITY_DENIED",
    "message": "Authorization rejected. Caller lacks [simulate] capability required to evaluate structural bounds."
  }
}
```

## 4. Default Behavior for Uncovered Commands
**Soft Degradation:** If `cmd.Annotations["qxctl_metadata"]` is absent (due to deliberate exclusion from the Covered Set), the `Authorize()` check passes automatically. 
*Reasoning:* This isolates execution blockades exclusively to the modernized AI tier, avoiding global operations outages during gradual migration.

## 5. Dry-Run vs Mutate Semantics
The `CommandMetadata` internally tracks `SupportsDryRun`. 
If a command (e.g., `CapDeploy`) allows dry-run evaluation AND the `--dry-run` or `--plan` flag is active upon invocation:
- The `Authorize` block systematically intercepts the dynamic state.
- The required evaluation capability downgrades from `deploy` directly to `simulate`.
- This provides guarantees that continuous integration bots can orchestrate "plans" without granting execution credentials.
