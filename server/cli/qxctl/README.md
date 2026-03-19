# QuanuX Developer CLI (`qxctl`) ⚡️

The official Go CLI interface representing the entire operational control plane for the high-frequency Cython-C++ QuanuX Core boundaries. 

## Build Instructions (Native)

The CLI adheres to strict `.goreleaser` configurations deployed dynamically to GitHub Actions.

```bash
# Verify library bounds completely natively
go mod tidy 

# Compile statically against Apple/Linux OS targets
make build

# Install the verified 59ns binary locally (Requires sudo)
make install 
```

## Architectural Roadmap Integrity

By converting all 25 core routing domains to Native Go interfaces via the Cobra AST format, `qxctl` securely prevents bash-layer exploits. All endpoints, including `spreader`, `crucible`, `foundry`, `telemetry`, and `orchestra`, natively bridge pure C++ / Protobuf interfaces across the system arrays.

### Features
* **Zero-Trust**: Vault PKI and Apple Keychain integrations firmly block rogue state injection. 
* **Zero-Copy**: Apache Arrow natively loads memoryframes to QuanuX containers bypassing socket barriers.
* **Elm Architecture**: Built securely utilizing standard `charmbracelet/bubbletea` constraints (no raw terminal cursors).

> For detailed deployment capabilities, read the physical CLI `man qxctl` documentation natively embedded into this executable layer.
