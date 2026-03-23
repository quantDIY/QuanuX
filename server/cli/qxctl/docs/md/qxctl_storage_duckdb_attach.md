## qxctl storage duckdb attach

Attach a DB file to the running QuanuX Server

```
qxctl storage duckdb attach [path] [alias] [flags]
```

### Options

```
  -h, --help                help for attach
      --server-url string   QuanuX Server URL (default "http://localhost:8000")
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

* [qxctl storage duckdb](qxctl_storage_duckdb.md)	 - Manage DuckDB cartridges statically

