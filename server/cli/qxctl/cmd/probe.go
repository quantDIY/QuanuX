package cmd

import (
	"github.com/QuanuX/qxctl/pkg/probe"
	"github.com/spf13/cobra"
)

var probeCmd = &cobra.Command{
	Use:   "probe",
	Short: "Neural Tap Diagnostic & Auto-Suture",
}

var probeExecuteCmd = &cobra.Command{
	Use:   "execute",
	Short: "Neural Tap: The cluster diagnostic stethoscope and surgical kit",
	RunE: func(cmd *cobra.Command, args []string) error {
		// Task 1: Command solely processes flags natively without global Viper state arrays.
		timeout, _ := cmd.Flags().GetInt("timeout")
		isFix, _ := cmd.Flags().GetBool("fix")

		App.Out.Log("INFO", "Delegating probe execution to service boundary via App container.")

		// Task 5: Context explicitly propagated.
		return probe.ExecuteDiagnostics(App.Ctx, timeout, isFix)
	},
}

func init() {
	rootCmd.AddCommand(probeCmd)
	probeCmd.AddCommand(probeExecuteCmd)

	// Dependency mapping rests on the execution block, Viper overrides abandoned natively.
	probeExecuteCmd.Flags().Bool("fix", false, "Execute Auto-Suture Protocol to repair critical systems")
	probeExecuteCmd.Flags().Int("timeout", 80, "Timeout for TCP dial in ms")
}
