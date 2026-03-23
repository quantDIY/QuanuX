## qxctl predeploy

Performs a Capability Handshake prior to deployment natively

```
qxctl predeploy [flags]
```

### Options

```
  -h, --help             help for predeploy
  -p, --payload string   Execution context payload
  -t, --target string    Node target array
      --type string      Deployment bounds mode (default "extension")
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

