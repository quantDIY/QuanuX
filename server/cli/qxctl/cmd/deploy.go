package cmd

import (
	"github.com/spf13/cobra"
	"github.com/QuanuX/qxctl/pkg/nest"
)

var rootDeployCmd = &cobra.Command{
	Use:   "deploy",
	Short: "Dynamic Habitat Deployment & Telemetry Lifecycle.",
	RunE: func(cmd *cobra.Command, args []string) error {
		return nest.Drop(cmd.Context(), "auto", "engine")
	},
}

func init() {
	rootCmd.AddCommand(rootDeployCmd)
}
