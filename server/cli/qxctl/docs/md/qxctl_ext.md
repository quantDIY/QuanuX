## qxctl ext

Manage QXP Extensions (List, Run)

### Options

```
  -h, --help   help for ext
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
* [qxctl ext clean](qxctl_ext_clean.md)	 - Remove logs and runtime files
* [qxctl ext enhance](qxctl_ext_enhance.md)	 - Trigger a 'Turbo' build with specialized performance options
* [qxctl ext install](qxctl_ext_install.md)	 - Build/Install the extension
* [qxctl ext integrate](qxctl_ext_integrate.md)	 - Inject a local proprietary SDK into the centralized extensions/sdks repository
* [qxctl ext list](qxctl_ext_list.md)	 - List all installed extensions and their status
* [qxctl ext manifest](qxctl_ext_manifest.md)	 - Emits the internal AI Command Metadata topology securely
* [qxctl ext remove](qxctl_ext_remove.md)	 - Alias for uninstall
* [qxctl ext run](qxctl_ext_run.md)	 - Run an extension in the foreground (injecting secrets safely extracted dynamically)
* [qxctl ext start](qxctl_ext_start.md)	 - Start an extension in the background
* [qxctl ext status](qxctl_ext_status.md)	 - Check if an extension is running
* [qxctl ext stop](qxctl_ext_stop.md)	 - Stop a running extension
* [qxctl ext uninstall](qxctl_ext_uninstall.md)	 - Clean up build artifacts (removes 'build' directory)
* [qxctl ext upgrade](qxctl_ext_upgrade.md)	 - Auto-upgrade to the latest version found upstream
* [qxctl ext upgradeable](qxctl_ext_upgradeable.md)	 - Check for available updates

