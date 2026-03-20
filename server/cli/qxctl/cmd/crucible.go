package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/crucible"
	"github.com/spf13/cobra"
)

func NewCrucibleCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "crucible",
		Short: "Manage QuanuX Crucible (Backtesting Studio)",
	}

	reportCmd := &cobra.Command{
		Use:   "report [strategy]",
		Short: "Retrieve instantaneous cache-aligned Backtest Metrics directly from the C++ Engine via Cython",
		RunE: func(cmd *cobra.Command, args []string) error {
			version, _ := cmd.Flags().GetString("version")
			return crucible.Report(app.Ctx, version)
		},
	}
	reportCmd.Flags().String("version", "1.0.0", "The version of the strategy.")

	reportAdvancedCmd := &cobra.Command{
		Use:   "report-advanced [strategy]",
		Short: "Retrieve Phase 5 Deep Statistical Analysis directly from the C++ Engine via Cython",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	reportAdvancedCmd.Flags().String("version", "1.0.0", "The version of the strategy.")
	reportAdvancedCmd.Flags().Int("mc-iterations", 1000, "Number of Monte Carlo resampling iterations.")

	startCmd := &cobra.Command{
		Use:   "start [strategy]",
		Short: "Start an isolated QuanuX Crucible backtest run",
		RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	startCmd.Flags().String("version", "1.0.0", "The version of the strategy.")

	statusCmd := &cobra.Command{
		Use:   "status", Short: "Check the status of the Crucible Engine", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}
	stopCmd := &cobra.Command{
		Use:   "stop", Short: "Stop the currently running Crucible backtest", RunE: func(cmd *cobra.Command, args []string) error { return nil },
	}

	cmd.AddCommand(reportCmd, reportAdvancedCmd, startCmd, statusCmd, stopCmd)
	return cmd
}
