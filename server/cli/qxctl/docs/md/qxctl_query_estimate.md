## qxctl query estimate

Transpiles AST and hits BigQuery Dry-Run API for metrics natively

```
qxctl query estimate [sql_file] [flags]
```

### Options

```
  -h, --help            help for estimate
  -t, --target string   Target analytics engine (default "bq")
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

* [qxctl query](qxctl_query.md)	 - Manage Analytical Extensions (Validate & Estimate SQL)

