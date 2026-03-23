## qxctl crucible report-advanced

Retrieve Phase 5 Deep Statistical Analysis directly from the C++ Engine via Cython

```
qxctl crucible report-advanced [strategy] [flags]
```

### Options

```
  -h, --help                help for report-advanced
      --mc-iterations int   Number of Monte Carlo resampling iterations. (default 1000)
      --version string      The version of the strategy. (default "1.0.0")
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

* [qxctl crucible](qxctl_crucible.md)	 - Manage QuanuX Crucible (Backtesting Studio)

