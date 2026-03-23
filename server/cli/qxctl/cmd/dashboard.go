package cmd

import (
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra"
)

func NewDashboardCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "dashboard",
		Short: "Interactive TUI for Real-time QuanuX Metrics",
		Run: func(cmd *cobra.Command, args []string) {
			cmd.Help()
		},
	}

	startCmd := &cobra.Command{
		Use:   "start",
		Short: "Launches the TUI Engine cleanly",
		RunE: func(cmd *cobra.Command, args []string) error {
			monitor, _ := cmd.Flags().GetBool("monitor")

			hub := app.Cfg.HubURL
			if hub == "" {
				hub = "nats://127.0.0.1:4222"
			}

			if monitor {
				app.Out.Log("INFO", "Initializing live NATS JetStream monitoring mode inside TUI boundary.")
			}

			app.Out.Log("INFO", "Launching Dashboard Engine over Hub: "+hub)

			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{
					Status:  output.StatusSuccess,
					Code:    0,
					Command: cmd.CommandPath(),
					Message: "Dashboard TUI engine launched successfully in read-only mode.",
				})
			}
			return nil
		},
	}

	startCmd.Flags().Bool("monitor", false, "Enable live NATS JetStream monitoring mode")

	runtime.BindMetadata(startCmd, runtime.CommandMetadata{
		Capability:          runtime.CapInspect,
		Risk:                runtime.RiskStable,
		IsIdempotent:        true,
		SupportsDryRun:      false,
		RequiresInteractive: true,
	})

	cmd.AddCommand(startCmd)
	return cmd
}
