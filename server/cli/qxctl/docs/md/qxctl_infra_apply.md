## qxctl infra apply

Runs Terraform Apply strictly for the designated deployment

```
qxctl infra apply [flags]
```

### Options

```
  -h, --help            help for apply
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

* [qxctl infra](qxctl_infra.md)	 - QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning

