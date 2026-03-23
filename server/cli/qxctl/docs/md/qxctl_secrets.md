## qxctl secrets

Manage API keys and secrets via OS Keyring natively

### Options

```
  -h, --help   help for secrets
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
* [qxctl secrets get](qxctl_secrets_get.md)	 - Retrieve and print a secret value safely
* [qxctl secrets list-keys](qxctl_secrets_list-keys.md)	 - List configured keys (hiding values)
* [qxctl secrets set](qxctl_secrets_set.md)	 - Set a specific secret key safely directly into Keyring targets
* [qxctl secrets setup](qxctl_secrets_setup.md)	 - Interactive wizard safely bound to outputs

