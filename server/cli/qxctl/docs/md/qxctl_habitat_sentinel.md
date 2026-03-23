## qxctl habitat sentinel

Deploys the strict Low-Overhead Sentinel Protocol (Telegraf & Promtail)

```
qxctl habitat sentinel [target] [flags]
```

### Options

```
  -h, --help                        help for sentinel
      --telemetry-interval string   Telemetry reporting interval for Telegraf (default "15s")
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

* [qxctl habitat](qxctl_habitat.md)	 - Manage QuanuX OS-level Habitat Provisioning

