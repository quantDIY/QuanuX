## qxctl engine

Manage QuanuX Edge Engine Tuning & Execution

### Options

```
  -h, --help   help for engine
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
* [qxctl engine setup-topology](qxctl_engine_setup-topology.md)	 - Configures the core JetStream QUANUX_INGEST stream and dual-consumer routing topology
* [qxctl engine start](qxctl_engine_start.md)	 - Initializes CNATS, Annex, and Omega Edge pipelines
* [qxctl engine tune](qxctl_engine_tune.md)	 - Invokes kernel and OS tuning playbooks for the designated architecture

