## qxctl bridge start

Start the SignalR bridge process

```
qxctl bridge start [flags]
```

### Options

```
  -h, --help             help for start
  -p, --port int         Port to bind the bridge to. (default 8077)
  -r, --runtime string   Runtime to use: 'flask' or 'node'. (default "flask")
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

* [qxctl bridge](qxctl_bridge.md)	 - Manage the SignalR Bridge

