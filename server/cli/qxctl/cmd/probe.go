package cmd

import (
	"github.com/QuanuX/qxctl/pkg/probe"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var probeCmd = &cobra.Command{
	Use:   "probe",
	Short: "Neural Tap Diagnostic & Auto-Suture",
	Run: func(cmd *cobra.Command, args []string) {
		cmd.Help()
	},
}

var probeProbeCmd = &cobra.Command{
	Use:   "probe",
	Short: "Neural Tap: The cluster diagnostic stethoscope and surgical kit",
	RunE: func(cmd *cobra.Command, args []string) error {
		timeout := viper.GetInt("probe.probe.probe.timeout")
		isFix := viper.GetBool("probe.probe.probe.fix")
		return probe.ExecuteDiagnostics(cmd.Context(), timeout, isFix)
	},
}

func init() {
	rootCmd.AddCommand(probeCmd)
	probeCmd.AddCommand(probeProbeCmd)
	probeProbeCmd.Flags().Bool("fix", false, "Execute Auto-Suture Protocol to repair critical systems")
	viper.BindPFlag("probe.probe.probe.fix", probeProbeCmd.Flags().Lookup("fix"))
	probeProbeCmd.Flags().Int("timeout", 80, "Timeout for TCP dial in ms")
	viper.BindPFlag("probe.probe.probe.timeout", probeProbeCmd.Flags().Lookup("timeout"))
}
