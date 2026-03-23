## qxctl foundry forge

Forge a new sovereign trading component via AI Generation

```
qxctl foundry forge [flags]
```

### Options

```
  -h, --help             help for forge
  -l, --lang string      Target language constraint (python, cython, cpp)
  -n, --name string      Name of the component
  -t, --type string      Type of component to forge (entry, exit, indicator, strategy)
  -v, --version string   Language version constraint (e.g. 3.11, 3.14, cpp20)
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

* [qxctl foundry](qxctl_foundry.md)	 - Manage QuanuX Foundry (Strategy Generator)

