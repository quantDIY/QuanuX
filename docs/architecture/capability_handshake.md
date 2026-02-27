# Capability Handshake Architecture

The QuanuX Ecosystem relies on a strict "Requirement vs. Provision" deployment lifecycle known as the Dynamic Habitat Handshake. This replaces legacy hardcoded node types (e.g., "Execution Node" vs "Observation Node").

## The Lifecycle

1. **Registry (`manifests/capability_registry.yaml`)**:
   The absolute source of truth mapping string capabilities (e.g. `isolcpus`) to physical Linux actions (e.g., `numactl --physcpubind={isolated_cores} --localalloc`).
2. **Habitat Provision (`/etc/quanux/habitat.json`)**:
   Stamped onto the file system by `node_init.sh` based on the targeted node Tier. It defines what physical resources the node *provides*.
3. **Payload Requirement (`requirements.json`)**:
   Authored alongside any deployable process, strictly defining a list of required capability strings from the registry.
4. **Wiring Hook**:
   When `quanuxctl deploy` executes, the capability strings dictate the execution prefix (via the registry's `wiring_hook`). The system dynamically assembles Linux command-line parameters to run the Inner Doll.

## Telemetry Handshake
A `quanuxctl predeploy` request triggers a NATS IPC exchange on `QUANUX.NODE.HANDSHAKE.<HOSTNAME>`. The target Native Envoy or Fast-Nerve validates that the payload's `requires` array is a strict subset of the Habitat's `provides` matrix before acknowledging deployment compatibility.
