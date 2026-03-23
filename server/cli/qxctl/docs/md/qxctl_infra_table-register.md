## qxctl infra table-register

Registers an external table against the data lake

```
qxctl infra table-register [flags]
```

### Options

```
  -h, --help             help for table-register
      --project string   GCP Project ID
      --target string    Cloud target (do or gcp) (default "gcp")
      --uri string       GCS URI for Parquet files
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

* [qxctl infra](qxctl_infra.md)	 - QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning

