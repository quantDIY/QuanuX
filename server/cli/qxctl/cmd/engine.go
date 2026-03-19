package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"github.com/QuanuX/qxctl/pkg/engine"
)

var engineCmd = &cobra.Command{
	Use:   "engine",
	Short: "Manage QuanuX Edge Engine Tuning & Execution",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("engine invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var engineSetupTopologyCmd = &cobra.Command{
	Use:   "setup-topology",
	Short: "Configures the core JetStream QUANUX_INGEST stream and dual-consumer routing topology",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("setup-topology invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var engineStartCmd = &cobra.Command{
	Use:   "start",
	Short: "Initializes CNATS, Annex, and Omega Edge pipelines",
	RunE: func(cmd *cobra.Command, args []string) error {
		hubUrl := viper.GetString("hub")
		if hubUrl == "" {
			hubUrl = "nats://127.0.0.1:4222"
		}
		target := viper.GetString("engine.engine.start.target")
		return engine.Start(cmd.Context(), hubUrl, target)
	},
}

var engineTuneCmd = &cobra.Command{
	Use:   "tune",
	Short: "Invokes kernel and OS tuning playbooks for the designated architecture",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("tune invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(engineCmd)
	engineCmd.AddCommand(engineSetupTopologyCmd)
	engineSetupTopologyCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("engine.engine.setup_topology.target", engineSetupTopologyCmd.Flags().Lookup("target"))
	engineCmd.AddCommand(engineStartCmd)
	engineStartCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("engine.engine.start.target", engineStartCmd.Flags().Lookup("target"))
	engineCmd.AddCommand(engineTuneCmd)
	engineTuneCmd.Flags().String("target", "gcp", "Infrastructure target (do or gcp)")
	viper.BindPFlag("engine.engine.tune.target", engineTuneCmd.Flags().Lookup("target"))
}

