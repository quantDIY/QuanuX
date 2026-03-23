## qxctl lifecycle

Dynamic Habitat Deployment & Telemetry Lifecycle

### Options

```
  -h, --help   help for lifecycle
```

### Options inherited from parent commands

```
      --config string   Explicit config file trajectory
      --hub string      QuanuX JetStream Engine Hub target (default "nats://127.0.0.1:4222")
      --output string   Set the output UX mode (text, json, yaml, quiet) (default "text")
      --trace           Enable extreme diagnostic observability bounds
      --verbose         Enable verbose payload outputs
```

### SEE ALSO

* [qxctl](qxctl.md)	 - QuanuX Control CLI (Go Edition)
* [qxctl lifecycle deploy](qxctl_lifecycle_deploy.md)	 - Deploys a payload into the outer shell (Habitat) safely using valid wiring hooks
* [qxctl lifecycle predeploy](qxctl_lifecycle_predeploy.md)	 - Performs a Capability Handshake prior to deployment
* [qxctl lifecycle ssh](qxctl_lifecycle_ssh.md)	 - Executes a payload via SSH, conditioned on the C++ Envoy generating the wrapper

