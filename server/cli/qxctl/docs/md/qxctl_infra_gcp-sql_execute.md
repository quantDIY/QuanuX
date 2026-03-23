## qxctl infra gcp-sql execute

Validates, transpiles, and executes bounded SQL against BigQuery

```
qxctl infra gcp-sql execute [query] [flags]
```

### Options

```
      --dry-run        Validate and transpile only, do not send to GCP
  -h, --help           help for execute
      --json           Emit purely JSON payload for machine execution
      --max-rows int   Maximum rows to fetch remotely (default 100)
      --timeout int    Timeout in seconds for remote execution (default 30)
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

