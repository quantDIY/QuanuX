## qxctl infra

QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning

### Options

```
  -h, --help   help for infra
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
* [qxctl infra apply](qxctl_infra_apply.md)	 - Runs Terraform Apply strictly for the designated deployment
* [qxctl infra auth](qxctl_infra_auth.md)	 - Outputs export commands for QECD Phase 3 Terraform Provisioning
* [qxctl infra destroy](qxctl_infra_destroy.md)	 - Destroys the designated QuanuX Deployment
* [qxctl infra gcp-sql](qxctl_infra_gcp-sql.md)	 - GCP Bounded AST SQL Transpilation
* [qxctl infra ingest-start](qxctl_infra_ingest-start.md)	 - Starts the QuanuX asynchronous ingestion pipeline
* [qxctl infra set-spaces-keys](qxctl_infra_set-spaces-keys.md)	 - Securely saves the DigitalOcean Spaces keys to the native OS Keyring
* [qxctl infra set-token](qxctl_infra_set-token.md)	 - Securely saves the DigitalOcean API Token to the native OS Keyring (Zero-Disk storage)
* [qxctl infra status](qxctl_infra_status.md)	 - Lists active nodes and vaults from terraform output
* [qxctl infra table-register](qxctl_infra_table-register.md)	 - Registers an external table against the data lake

