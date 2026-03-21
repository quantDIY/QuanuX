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
	return &cobra.Command{
		Use:   "manifest",
		Short: "Emits the internal AI Command Metadata topology securely",
		RunE: func(cmd *cobra.Command, args []string) error {
			var commands []runtime.CommandMetadata

			coveredCommands := map[string]bool{
				"qxctl vault status":   true,
				"qxctl query validate": true,
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
				Status: output.StatusSuccess,
				Code:   0,
				Data:   env,
			})
			return nil
		},
	}
}
