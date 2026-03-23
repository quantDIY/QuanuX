## qxctl ext enhance

Trigger a 'Turbo' build with specialized performance options

```
qxctl ext enhance [name] [flags]
```

### Options

```
      --allocator string   Memory allocator: system, jemalloc, mimalloc (default "system")
  -h, --help               help for enhance
      --logger string      Logging strategy: file, async, null (default "file")
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

* [qxctl ext](qxctl_ext.md)	 - Manage QXP Extensions (List, Run)

