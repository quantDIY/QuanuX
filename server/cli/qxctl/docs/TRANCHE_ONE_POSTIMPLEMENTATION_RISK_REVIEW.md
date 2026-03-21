# Tranche One Post-Implementation Risk Review

This document audits the remaining leak paths and structural risks post-Tranche One natively.

## 1. Bypass Paths
- **Envelope Bypass:** Commands falling outside the strict `vault status` and `query validate` Covered Set continue to bypass `app.Out.PrintJSON` entirely on success loops natively, reverting to unmanaged human layouts.
- **Redaction Bypass:** Any external SDK or third-party package (e.g., `terraform`) that circumvents the `ux.Manager` natively by hard-writing to `os.Stdout` escapes the `security.Redact()` perimeter untouched.
- **Cobra Stderr Writes:** Framework errors still emit the literal `Error: unknown command...` string natively to standard error before `main.go` intercepts it to fire the JSON envelope on standard out.

## 2. Panic & Signal Handling
- If a hard panic occurs (e.g., dereferencing `nil`), `main.go` does not natively catch it via `recover()`. The Go runtime bypasses all `OutputEnvelope` wrappers entirely natively, ejecting raw stack traces to `os.Stderr`.

## 3. Explicitly Deferred Legacy Commands
- The remaining 56 leaf commands operate entirely on legacy paths until authorized for migration.

## 4. Expansion Breakage Risks
- If the Covered Set expands prematurely into `node deploy` or `infra apply` without specifically binding and blocking the subprocess stream leaks, JSON logs would be irreparably corrupted organically by intertwined OS text streams blocking all automated parsers.
