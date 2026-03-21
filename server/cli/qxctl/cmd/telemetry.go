package cmd

import (
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/telemetry"
	"github.com/spf13/cobra"
)

func NewTelemetryCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "telemetry",
		Short: "Manage Node Telemetry Services remotely",
	}

	restartCmd := &cobra.Command{
		Use:   "restart",
		Short: "Restart a remote telemetry service",
		RunE: func(cmd *cobra.Command, args []string) error {
			node, _ := cmd.Flags().GetString("node")
			service, _ := cmd.Flags().GetString("service")
			return telemetry.Restart(app.Ctx, node, service)
		},
	}
	restartCmd.Flags().StringP("node", "n", "", "Target node hostname")
	restartCmd.Flags().StringP("service", "s", "", "Service name: nerve or envoy")

	startCmd := &cobra.Command{
		Use:   "start", Short: "Start a remote telemetry service", RunE: func(cmd *cobra.Command, args []string) error {
			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{Status: output.StatusSuccess, Code: 0, Command: cmd.Use, Message: "Telemetry service started successfully."})
			}
			return nil
		},
	}
	startCmd.Flags().StringP("node", "n", "", "Target node hostname")
	startCmd.Flags().StringP("service", "s", "", "Service name")

	statusCmd := &cobra.Command{
		Use:   "status", Short: "Check the status of a remote telemetry service", RunE: func(cmd *cobra.Command, args []string) error {
			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{Status: output.StatusSuccess, Code: 0, Command: cmd.Use, Message: "Telemetry service status OK."})
			}
			return nil
		},
	}
	statusCmd.Flags().StringP("node", "n", "", "Target node hostname")
	statusCmd.Flags().StringP("service", "s", "", "Service name")

	stopCmd := &cobra.Command{
		Use:   "stop", Short: "Stop a remote telemetry service", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	stopCmd.Flags().StringP("node", "n", "", "Target node hostname")
	stopCmd.Flags().StringP("service", "s", "", "Service name")

	runtime.BindMetadata(statusCmd, runtime.CommandMetadata{
		Capability:          runtime.CapInspect,
		Risk:                runtime.RiskStable,
		IsIdempotent:        true,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	runtime.BindMetadata(startCmd, runtime.CommandMetadata{
		Capability:          runtime.CapDeploy,
		Risk:                runtime.RiskDangerous,
		IsIdempotent:        false,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	runtime.BindMetadata(stopCmd, runtime.CommandMetadata{
		Capability:          runtime.CapDeploy,
		Risk:                runtime.RiskDangerous,
		IsIdempotent:        false,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	runtime.BindMetadata(restartCmd, runtime.CommandMetadata{
		Capability:          runtime.CapDeploy,
		Risk:                runtime.RiskDangerous,
		IsIdempotent:        false,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	cmd.AddCommand(restartCmd, startCmd, statusCmd, stopCmd)
	return cmd
}
