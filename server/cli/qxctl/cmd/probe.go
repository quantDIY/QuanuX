package cmd

import (
	"github.com/QuanuX/qxctl/internal/runtime"
	"github.com/QuanuX/qxctl/pkg/probe"
	"github.com/spf13/cobra"
)

func NewProbeCmd(app *runtime.App) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "probe",
		Short: "Neural Tap Diagnostic & Auto-Suture",
	}

	executeCmd := &cobra.Command{
		Use:   "execute",
		Short: "Neural Tap: The cluster diagnostic stethoscope and surgical kit",
		RunE: func(cmd *cobra.Command, args []string) error {
			timeout, _ := cmd.Flags().GetInt("timeout")
			isFix, _ := cmd.Flags().GetBool("fix")

			app.Out.Log("INFO", "Delegating probe execution to service boundary via App container.")
			return probe.ExecuteDiagnostics(app.Ctx, timeout, isFix)
		},
	}
	executeCmd.Flags().Bool("fix", false, "Execute Auto-Suture Protocol to repair critical systems")
	executeCmd.Flags().Int("timeout", 80, "Timeout for TCP dial in ms")

	cmd.AddCommand(executeCmd)
	return cmd
}
