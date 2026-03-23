## qxctl vault status

Interrogates the live Annex C++ daemon bounding the Sovereign Vault and NVMe chunk cache

```
qxctl vault status [flags]
```

### Options

```
  -h, --help            help for status
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

* [qxctl vault](qxctl_vault.md)	 - Manage QuanuX Sovereign Vault and Local HW Cache

