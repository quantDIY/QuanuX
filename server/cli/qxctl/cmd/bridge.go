package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/bridge"
	"github.com/spf13/cobra"
)

func NewBridgeCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "bridge",
		Short: "Manage the SignalR Bridge",
	}

	startCmd := &cobra.Command{
		Use:   "start",
		Short: "Start the SignalR bridge process",
		RunE: func(cmd *cobra.Command, args []string) error {
			runtimeName, _ := cmd.Flags().GetString("runtime")
			port, _ := cmd.Flags().GetInt("port")
			return bridge.Start(app.Ctx, runtimeName, port)
		},
	}
	startCmd.Flags().StringP("runtime", "r", "flask", "Runtime to use: 'flask' or 'node'.")
	startCmd.Flags().IntP("port", "p", 8077, "Port to bind the bridge to.")

	stopCmd := &cobra.Command{
		Use:   "stop",
		Short: "Stop the SignalR bridge (placeholder)",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	cmd.AddCommand(startCmd, stopCmd)
	return cmd
}
