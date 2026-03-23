## qxctl node deploy

Deploy the QuanuX Execution Node to a remote server via SSH

```
qxctl node deploy [target] [flags]
```

### Options

```
      --dry-run        Print command without executing
  -h, --help           help for deploy
      --hub string     NATS Hub URL (Falls back to app trajectory by default)
      --token string   Registration Token
```

### Options inherited from parent commands

```
      --config string   Explicit config file trajectory
      --output string   Set the output UX mode (text, json, yaml, quiet) (default "text")
      --trace           Enable extreme diagnostic observability bounds
      --verbose         Enable verbose payload outputs
```

### SEE ALSO

* [qxctl node](qxctl_node.md)	 - Manage Remote Execution Nodes (Deploy, List)

