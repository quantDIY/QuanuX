# QuanuX CLI (`qxctl`) Plugin Host Contract

## Architecture Boundary
QuanuX operates an **Airgapped Sidecar Topology** utilizing `hashicorp/go-plugin` natively. Sidecars map into physical bin limits (e.g. `qxctl-cython`, `qxctl-omega`) avoiding Python interpreter crashes natively over Memory Sockets (gRPC).

## Explicit Handshake Standard
To prevent rogue binary injection or memory poisoning, the `qxctl` orchestrator mandates a strict handshake signature dynamically during the socket initialization. 

Any physical binary attempting to bind to `qxctl` MUST authenticate via:

```go
plugin.HandshakeConfig{
	ProtocolVersion:  1,
	MagicCookieKey:   "QUANUX_PLUGIN_SYSTEM",
	MagicCookieValue: "QUANUX_AIRGAP_SECURE",
}
```

## Sidecar Construction (C++ & Go)
All sidecars map to standard Protobuf descriptors handling `net/rpc` or `grpc`. The executable MUST NOT emit raw CLI output except across standard bounded RPC log loops. 

1. Executable nomenclature MUST precisely mirror: `qxctl-<boundary>`.
2. Binary must map to `$PATH` organically supporting 0 dependency mapping externally. 
3. Outputs are captured strictly mapping standard `security.Redactor` filters enforcing native Zero-Trust paths.
