---
name: QuanuX CLI Engine (qxctl)
description: Complete specification of the Native Go CLI Engine representing the QuanuX Hardware Bounds.
---

# QuanuX CLI (`qxctl`) Architecture

The new Native Go `qxctl` executable formally decapitates the legacy Python Typer shell-scripting architecture, migrating all QuanuX execution boundaries deeply into the physical Go Memory Limits inside isolated `pkg/` interfaces.

## Capabilities (Version 1.0.0 Natively Rebounded)

The CLI no longer shells out to `bash` for operational capabilities. It is fully self-contained using memory-mapped array limits and native libraries.

1. **Hardware Telemetry (`cmd/probe`, `cmd/telemetry`)**
   - Implements native Go `net.Dial` and `sync.WaitGroup` concurrency to interrogate local sockets instantly.
   - Retrieves Apple MacOS Keychain values privately into zero-trust bytes using `zalando/go-keyring`.
   - Relies on OpenTelemetry mesh networking dynamically.

2. **Security & Cryptography (`pkg/secrets`, `pkg/vault`)**
   - Directly binds token allocations into `hashicorp/vault/api` dynamically, ignoring hardcoded tokens.
   - AES-GCM stream capabilities natively built-in utilizing pure Go OS pointers.

3. **High-Frequency Engineering (`pkg/engine`, `pkg/storage`, `pkg/spreader`)**
   - NATS JetStream strictly mapped without subprocesses publishing directly into C++ Cython loop arrays.
   - Arrow IPC (`apache/arrow/go`) natively managing zero-copy CGO frames avoiding Python GIL limitations entirely.
   - FIX Routing Engine strictly bound to the `.quanux/` file limits utilizing `github.com/quickfixgo/quickfix`.

4. **Production Cloud Deployments (`pkg/infra`, `pkg/orchestra`)**
   - Remote Node isolation managed autonomously over pure native `golang.org/x/crypto/ssh` bindings.
   - Hardware Infrastructure completely pipelined natively using HashiCorp `terraform-exec` (TF API integration).

## Design Philosophy
Absolute adherence to statically compiled Go limits. Zero Python instances are spawned dynamically. The `qxctl` CLI represents the absolute highest source of truth for physical QuanuX bounds.
