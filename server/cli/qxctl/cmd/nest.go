package cmd

import (
	"github.com/QuanuX/qxctl/internal/output"
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
			if err := nest.Drop(app.Ctx, args[0], engine); err != nil {
				return err
			}
			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{Status: output.StatusSuccess, Code: 0, Command: cmd.CommandPath(), Message: "C++ Engine deployed successfully."})
			}
			return nil
		},
	}
	dropCmd.Flags().String("engine", "spreader", "Engine type to compile and bind")

	runtime.BindMetadata(dropCmd, runtime.CommandMetadata{Capability: runtime.CapDeploy, Risk: runtime.RiskDangerous, IsIdempotent: false, SupportsDryRun: false, RequiresInteractive: false})

	cmd.AddCommand(dropCmd)
	return cmd
}
