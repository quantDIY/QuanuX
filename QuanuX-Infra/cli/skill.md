---
description: QuanuX CLI operations, OS/Application boundaries, and the Habitat pattern
---

# QuanuX CLI (`quanuxctl`) Architecture

The QuanuX CLI is the centralized operational nervous system for managing the global matrix. It enforces a strict separation of concerns between OS-level infrastructure conditioning (Habitat) and application-level binaries (Nest).

## The Habitat vs. Nest Doctrine

To maintain zero latency degradation and immutable physical infrastructure, QuanuX rigorously splits deployments into two distinct stages:

### Stage 1: The Habitat
*   **Command**: `quanuxctl habitat equip <target>`
*   **Scope**: Bare metal conditioning only.
*   **Artifacts**:
    *   OS Packages (C++20 compilers, CMake, Ninja)
    *   Native Library Dependencies (ZeroMQ, OpenSSL, DuckDB headers)
    *   OS-level IPC parameters and memory maps
    *   The sacred `/etc/quanux/habitat.env` file.
*   **Philosophy**: The OS is blind to the QuanuX application. It simply prepares the exact soil and root system required.

### Stage 2: The Nest
*   **Command**: `quanuxctl nest drop <target>`
*   **Scope**: Application binaries and sovereign engine deployments only.
*   **Artifacts**:
    *   Pre-compiled `QuanuX-Spreader` or C++ logic binaries.
    *   SystemD execution wrappers targeting `/opt/quanux/bin`.
*   **Philosophy**: The Nest is blind to the deployment orchestrator. When it boots, it drinks blindly from `/etc/quanux/habitat.env`. If the bindings are wrong, it intentionally dies.

## The Environment Handshake (`habitat.env`)

The true mathematical beauty of the CLI rests in how Ansible translates cloud matrix dynamics into static C++ execution parameters without requiring hardcoded technical debt.
When the Habitat is equipped, Ansible dynamically extracts the internal VPC IP of the `panopticon_buffer` (where NATS JetStream resides) utilizing robust dynamic inventory `hostvars` templating and writes it directly into the `habitat.env` file.

The `Nest` SystemD service then binds exactly to `EnvironmentFile=/etc/quanux/habitat.env`, ensuring the C++ processes connect directly to the active internal mesh without requiring runtime DNS resolution or dynamic discovery.
