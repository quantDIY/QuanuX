## qxctl telemetry status

Check the status of a remote telemetry service

```
qxctl telemetry status [flags]
```

### Options

```
  -h, --help             help for status
  -n, --node string      Target node hostname
  -s, --service string   Service name
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

* [qxctl telemetry](qxctl_telemetry.md)	 - Manage Node Telemetry Services remotely

