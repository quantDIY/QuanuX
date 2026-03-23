## qxctl orchestra compile

Executes standardizer_cli, generating the C++ constexpr bridge, the Cython wrappers, and embedding the checksums

```
qxctl orchestra compile [flags]
```

### Options

```
  -h, --help           help for compile
  -v, --venue string   The broker venue to compile (e.g. ibkr)
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

* [qxctl orchestra](qxctl_orchestra.md)	 - Manage QuanuX-Orchestra Universal Naming Registry

