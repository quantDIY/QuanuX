---
name: QuanuX CLI Engine (qxctl)
description: AI Agent Command Reference and Native Go Execution Parameters
---

# QuanuX CLI (`qxctl`) Architecture

The `qxctl` CLI is the physical QuanuX hardware orchestrator natively executing bounded interactions manually tracing system arrays.

## 🚨 AI Agent Primary Directives 🚨

When you are directed to operate, interrogate, or mutate the `qxctl` state space, you **must strictly adhere** to these principles:

1. **Always Request JSON Output.** Provide `--output=json` systematically against Covered endpoints to receive deterministic parsing parameters.
2. **Consult the AI Reference.** The complete metadata and execution limits exist locally in `docs/AI_COMMAND_REFERENCE.md`.
3. **Use the Machine-Assisted Docs.** Every single parameter, flag, and command tree natively exists inside the generated `docs/md/` and `docs/man/` repositories.

## Capabilities & Bounding

Commands map physically to execution nodes natively:
- **`CapInspect`**: Safe, observational telemetry.
- **`CapDeploy`**: Hardware mutations and Cython grid modifications perfectly bounded physically cleanly.

If you encounter `CAPABILITY_DENIED` blocks, you objectively lack the correctly bound Vault token.

## Known Limitations and Bypasses
The CLI is actively undergoing phase expansions. Certain nodes (like `orchestra compile` or `topstepx apikey`) remain fundamentally unmapped, acting as **bypassed loops** ignoring JSON formatting natively.

*(Consult `docs/AI_COMMAND_REFERENCE.md` for explicit lists natively.)*
