package cmd

import (
	"encoding/json"

	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra"
)

type ManifestEnvelope struct {
	Version  string                    `json:"version"`
	Commands []runtime.CommandMetadata `json:"commands"`
}

func NewManifestCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "manifest",
		Short: "Emits the internal AI Command Metadata topology securely",
		RunE: func(cmd *cobra.Command, args []string) error {
			var commands []runtime.CommandMetadata

			coveredCommands := map[string]bool{
				// Legacy Core
				"qxctl vault status":     true,
				"qxctl query validate":   true,
				"qxctl secrets get":      true,
				"qxctl spreader package": true,
				"qxctl node deploy":      true,

				// Tranche 4B Option 1 (Analytical)
				"qxctl telemetry status":  true,
				"qxctl telemetry start":   true,
				"qxctl telemetry stop":    true,
				"qxctl telemetry restart": true,
				"qxctl dashboard start":   true,

				// Tranche 4B Option 2 Stage 1 (Storage)
				"qxctl storage scan":          true,
				"qxctl storage duckdb attach": true,
				"qxctl storage duckdb detach": true,
				"qxctl storage duckdb init":   true,

				// Tranche 4B Option 2 Stage 2 (Nest)
				"qxctl nest drop": true,

				// Tranche 4B Option 2 Stage 3 (Ext)
				"qxctl ext manifest":    true,
				"qxctl ext list":        true,
				"qxctl ext status":      true,
				"qxctl ext upgradeable": true,
				"qxctl ext clean":       true,
				"qxctl ext enhance":     true,
				"qxctl ext install":     true,
				"qxctl ext integrate":   true,
				"qxctl ext remove":      true,
				"qxctl ext run":         true,
				"qxctl ext start":       true,
				"qxctl ext stop":        true,
				"qxctl ext uninstall":   true,
				"qxctl ext upgrade":     true,
			}

			var walk func(*cobra.Command)
			walk = func(c *cobra.Command) {
				path := c.CommandPath()
				if coveredCommands[path] {
					if metaStr, ok := c.Annotations["qxctl_metadata"]; ok {
						var meta runtime.CommandMetadata
						if err := json.Unmarshal([]byte(metaStr), &meta); err == nil {
							meta.SchemaRef = path
							commands = append(commands, meta)
						}
					}
				}
				for _, child := range c.Commands() {
					walk(child)
				}
			}
			walk(cmd.Root())

			env := ManifestEnvelope{Version: "1.0", Commands: commands}
			app.Out.PrintJSON(output.OutputEnvelope{
				Status:  output.StatusSuccess,
				Code:    0,
				Command: cmd.CommandPath(),
				Data:    env,
			})
			return nil
		},
	}
	runtime.BindMetadata(cmd, runtime.CommandMetadata{Capability: runtime.CapInspect, Risk: runtime.RiskStable, IsIdempotent: true, SupportsDryRun: false, RequiresInteractive: false})
	return cmd
}
