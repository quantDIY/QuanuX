# Tranche Two Boundary Hardening

This document defines the wrapper strategies required to tame the remaining unmanaged text boundaries polluting JSON streams.

## 1. Subprocess Stderr/Stdout Handling Strategy
Arbitrary subprocesses (e.g., `terraform`, `ssh`) bypass the CLI IO manager entirely when connected directly to `os.Stdout`.
**Strategy:**
- Commands promoted into the Tranche Two Covered Set must refactor `os/exec.Cmd` allocations.
- `cmd.Stdout` and `cmd.Stderr` must be detached from `os.Stdout`/`os.Stderr`.
- Outputs must be swept into buffered `bytes.Buffer` instances.
- Upon success, standard logs are either silenced globally or structurally bound to `OutputEnvelope.Data`.
- Upon failure, the `bytes.Buffer.String()` is serialized into the `OutputEnvelope.Error.SubProcessLog` string, securing the terminal entirely.

## 2. Panic Containment Strategy
**Strategy:**
- Implement a `defer func()` recovery block encircling `root.ExecuteContext()` exclusively inside `main.go`.
- If a panic triggers, the recovery block casts the `recover()` array into a formal `OutputEnvelope` possessing Exit Code `500` (`FATAL_PANIC`).
- This guarantees even nil-pointer dereferences execute flawlessly across the JSON barrier, maintaining CI parser resilience.

## 3. Remaining Redaction Bypasses
- The only remaining leak path strictly comprises explicitly unmanaged Go routines writing directly to the `os.Stdout` pointers or hard `os.Exit()` commands fired improperly from embedded third-party C/CGO binaries (like QuanuX C++ Annex engine instances) which terminate the thread before the defer evaluates.

## 4. Unsolvable Limits (Tranche Two)
**Out-of-Scope:** We cannot suppress standard external daemon segmentation faults terminating the process bounds out-of-band. We can only suppress `qxctl` originating string outputs within the Go runtime thread limitations.
