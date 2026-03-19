package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var probeCmd = &cobra.Command{
	Use:   "probe",
	Short: "Neural Tap Diagnostic & Auto-Suture",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("probe invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var probeProbeCmd = &cobra.Command{
	Use:   "probe",
	Short: "Neural Tap: The cluster diagnostic stethoscope and surgical kit",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("probe invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(probeCmd)
	probeCmd.AddCommand(probeProbeCmd)
	probeProbeCmd.Flags().Bool("fix", false, "Execute Auto-Suture Protocol to repair critical systems")
	viper.BindPFlag("probe.probe.probe.fix", probeProbeCmd.Flags().Lookup("fix"))
}

