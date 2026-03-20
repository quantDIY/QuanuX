package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/nest"
	"github.com/spf13/cobra"
)

func NewNestCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "nest",
		Short: "Manage QuanuX C++ Sovereign Engine Nest Deployment",
	}

	dropCmd := &cobra.Command{
		Use:   "drop [target]",
		Short: "Deploys the C++ execution logic into a pre-conditioned OS Habitat",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			engine, _ := cmd.Flags().GetString("engine")
			return nest.Drop(app.Ctx, args[0], engine)
		},
	}
	dropCmd.Flags().String("engine", "spreader", "Engine type to compile and bind")

	cmd.AddCommand(dropCmd)
	return cmd
}
