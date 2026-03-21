package cmd

import (
	"os/exec"

	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/spreader"
	cliExec "github.com/QuanuX/qxctl/internal/exec"
	"github.com/QuanuX/qxctl/internal/output"
	"github.com/spf13/cobra"
)

func NewSpreaderCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "spreader",
		Short: "Manage QuanuX-Spreader Engine Deployment and Strategy Injection natively",
	}

	deployCmd := &cobra.Command{
		Use:   "deploy [binary_name]",
		Short: "Push the compiled Spreader to the Execution Node stub cleanly executing Checksums",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			return spreader.Deploy(app.Ctx, args[0])
		},
	}

	packageCmd := &cobra.Command{
		Use:   "package [strategy_json]",
		Short: "Transpile the strategy into C++ native engine bounds",
		RunE:  func(cmd *cobra.Command, args []string) error {
			sub := exec.CommandContext(app.Ctx, "echo", "Mocking C++ transpilation successfully via bounded buffers...")
			bOut, err := cliExec.BoundedRun(app.Ctx, sub)
			if err != nil {
				return err
			}
			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{
					Status:  output.StatusSuccess,
					Code:    0,
					Command: "spreader package",
					Data:    bOut,
				})
			}
			return nil
		},
	}

	runtime.BindMetadata(packageCmd, runtime.CommandMetadata{
		Capability:          runtime.CapSimulate,
		Risk:                runtime.RiskStable,
		IsIdempotent:        true,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})

	cmd.AddCommand(deployCmd, packageCmd)
	return cmd
}
