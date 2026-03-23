## qxctl orchestra

Manage QuanuX-Orchestra Universal Naming Registry

### Options

```
  -h, --help   help for orchestra
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
* [qxctl orchestra bootstrap](qxctl_orchestra_bootstrap.md)	 - Executes the Python script to download the ISO 20022 XML and generate constants
* [qxctl orchestra compile](qxctl_orchestra_compile.md)	 - Executes standardizer_cli, generating the C++ constexpr bridge, the Cython wrappers, and embedding the checksums
* [qxctl orchestra verify](qxctl_orchestra_verify.md)	 - Checks parity between active checksums safely

