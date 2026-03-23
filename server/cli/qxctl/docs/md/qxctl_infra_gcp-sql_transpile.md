## qxctl infra gcp-sql transpile

Emits translated BigQuery SQL if within the approved Phase 1 matrix

```
qxctl infra gcp-sql transpile [query] [flags]
```

### Options

```
  -h, --help   help for transpile
      --json   Emit purely JSON payload for machine execution
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

* [qxctl infra gcp-sql](qxctl_infra_gcp-sql.md)	 - GCP Bounded AST SQL Transpilation

