# Tranche Three Post-Implementation Risk Review

This document audits the remaining leak paths and structural risks post-Tranche Three.

## 1. Panic & Signal Handling Bypasses
- **Hard VM Terminations:** The global panic recovery wrapped around `ExecuteContext()` handles native Go panics safely. OS Signals (`SIGINT`, `SIGTERM`) route securely into structured envelopes via `infra.WatchTermination`. However, `SIGKILL` (`kill -9`) or catastrophic VM hardware resets (e.g., OOM killer termination) cannot be intercepted by the Go runtime and will instantly sever the process without emitting a JSON validation envelope. 

## 2. Subprocess Capture Bypasses
- **Detached Daemons:** The `cliExec.BoundedRun` wrapper safely intercepts and manages streams bounded correctly to robust file matrices. However, subprocesses that intentionally invoke `fork` routines disconnecting their execution or completely detach their streams from `Stdout`/`Stderr` logically prior to payload emission will bypass capturing completely.

## 3. Spill Path Redactions
- **Unredacted Disk Spills:** To combat high memory faults safely, bounded subprocess outputs exceeding 1MB buffer to local disk (e.g., `/tmp/qxctl-spill-*.log`). The spilled file content resting directly on disk is **not redacted**. While the programmatic `output.OutputEnvelope` strictly redacts sensitive CLI logging traversing memory via `security.Redact()`, the physical spill file drops unmodified unredacted byte streams out locally securely enforcing 100% telemetry completeness for offline debugging.

## 4. Vault OIDC Mock Constraints
- **Network Resolution Missing:** Our current logic in `auth.go` intercepts native JSON payload extraction from strings effectively decoding base64 boundaries. We do decode exact JWT primitives conforming to standard schemas natively. However, this is isolated to parsing. Cryptographic signature verification (JWKS verification headers) via physical Vault networks is currently completely mocked natively. Local validations explicitly unblock pilot validation locally ensuring logical branching resolves securely without latency or VPN interlocks.

## 5. Trust-Source Rollout Blockers
- **Integration Blockade:** Broader trust-source rollout extending into physical prod structures cannot commence securely until the structural OIDC verification correctly fetches Vault JWT webkeys confirming origin integrity and strictly handling token obsolescence via remote Vault caching algorithms.
