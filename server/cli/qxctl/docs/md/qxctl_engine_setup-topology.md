## qxctl engine setup-topology

Configures the core JetStream QUANUX_INGEST stream and dual-consumer routing topology

```
qxctl engine setup-topology [flags]
```

### Options

```
  -h, --help            help for setup-topology
      --target string   Infrastructure target (do or gcp) (default "gcp")
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

* [qxctl engine](qxctl_engine.md)	 - Manage QuanuX Edge Engine Tuning & Execution

