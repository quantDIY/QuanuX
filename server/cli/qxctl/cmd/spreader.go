package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/spreader"
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
		RunE:  func(cmd *cobra.Command, args []string) error { return nil },
	}

	cmd.AddCommand(deployCmd, packageCmd)
	return cmd
}
