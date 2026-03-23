## qxctl infra set-token

Securely saves the DigitalOcean API Token to the native OS Keyring (Zero-Disk storage)

```
qxctl infra set-token [token] [flags]
```

### Options

```
  -h, --help            help for set-token
      --target string   Target bound to evaluate against. (default "gcp")
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

