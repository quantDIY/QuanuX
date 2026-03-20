package cmd

import (
	"github.com/QuanuX/qxctl/pkg/nest"
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/spf13/cobra"
)

func NewDeployCmd(app *runtime.App) *cobra.Command {
	return &cobra.Command{
		Use:   "deploy",
		Short: "Dynamic Habitat Deployment & Telemetry Lifecycle.",
		RunE: func(cmd *cobra.Command, args []string) error {
			return nest.Drop(cmd.Context(), "auto", "engine")
		},
	}
}
