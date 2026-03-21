# Tranche Two Post-Implementation Risk Review

This document audits the remaining leak paths and structural risks post-Tranche Two.

## 1. Panic & Signal Handling
- The `PersistentPreRunE` implementation does not explicitly recover panics yet. A hard segment fault terminates the runtime bypassing the JSON format. This must be solved in Tranche Three global recovery boundaries.

## 2. Subprocess Scaling Risks
- Buffer pipes in `internal/exec.BoundedRun` currently use memory streams directly. Deploying this wrapper broadly across high-bandwidth tasks (like downloading GB-scale AI models) will execute OOM faults rapidly. Future tranches must stream to disk or limit array depths. Additionally, the bounded subprocess path output is not redacted before envelope emission; standard output is captured and emitted directly within the JSON envelope.

## 3. Implicit Auth Trust
- The `internal/runtime/auth.go` intercepts currently evaluate relying upon a mocked token struct natively. Full integration with Vault OIDC models or NATS JetStream auth servers remains incomplete, representing a blockade for physical production operations.
