## qxctl lifecycle ssh

Executes a payload via SSH, conditioned on the C++ Envoy generating the wrapper

```
qxctl lifecycle ssh [target] [payload_name] [flags]
```

### Options

```
  -h, --help   help for ssh
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

* [qxctl lifecycle](qxctl_lifecycle.md)	 - Dynamic Habitat Deployment & Telemetry Lifecycle

