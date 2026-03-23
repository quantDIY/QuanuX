## qxctl lifecycle predeploy

Performs a Capability Handshake prior to deployment

```
qxctl lifecycle predeploy [flags]
```

### Options

```
  -h, --help             help for predeploy
  -p, --payload string   Path to payload directory
  -t, --target string    Target Node Hostname
      --type string      Payload Type (default "extension")
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

