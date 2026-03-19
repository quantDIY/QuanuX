package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var telemetryCmd = &cobra.Command{
	Use:   "telemetry",
	Short: "Manage Node Telemetry Services remotely",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("telemetry invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var telemetryRestartCmd = &cobra.Command{
	Use:   "restart",
	Short: "Restart a remote telemetry service",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("restart invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var telemetryStartCmd = &cobra.Command{
	Use:   "start",
	Short: "Start a remote telemetry service",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("start invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var telemetryStatusCmd = &cobra.Command{
	Use:   "status",
	Short: "Check the status of a remote telemetry service",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("status invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var telemetryStopCmd = &cobra.Command{
	Use:   "stop",
	Short: "Stop a remote telemetry service",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("stop invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(telemetryCmd)
	telemetryCmd.AddCommand(telemetryRestartCmd)
	telemetryRestartCmd.Flags().StringP("node", "n", "", "Target node hostname")
	viper.BindPFlag("telemetry.telemetry.restart.node", telemetryRestartCmd.Flags().Lookup("node"))
	telemetryRestartCmd.Flags().StringP("service", "s", "", "Service name: nerve or envoy")
	viper.BindPFlag("telemetry.telemetry.restart.service", telemetryRestartCmd.Flags().Lookup("service"))
	telemetryCmd.AddCommand(telemetryStartCmd)
	telemetryStartCmd.Flags().StringP("node", "n", "", "Target node hostname")
	viper.BindPFlag("telemetry.telemetry.start.node", telemetryStartCmd.Flags().Lookup("node"))
	telemetryStartCmd.Flags().StringP("service", "s", "", "Service name: nerve or envoy")
	viper.BindPFlag("telemetry.telemetry.start.service", telemetryStartCmd.Flags().Lookup("service"))
	telemetryCmd.AddCommand(telemetryStatusCmd)
	telemetryStatusCmd.Flags().StringP("node", "n", "", "Target node hostname")
	viper.BindPFlag("telemetry.telemetry.status.node", telemetryStatusCmd.Flags().Lookup("node"))
	telemetryStatusCmd.Flags().StringP("service", "s", "", "Service name: nerve or envoy")
	viper.BindPFlag("telemetry.telemetry.status.service", telemetryStatusCmd.Flags().Lookup("service"))
	telemetryCmd.AddCommand(telemetryStopCmd)
	telemetryStopCmd.Flags().StringP("node", "n", "", "Target node hostname")
	viper.BindPFlag("telemetry.telemetry.stop.node", telemetryStopCmd.Flags().Lookup("node"))
	telemetryStopCmd.Flags().StringP("service", "s", "", "Service name: nerve or envoy")
	viper.BindPFlag("telemetry.telemetry.stop.service", telemetryStopCmd.Flags().Lookup("service"))
}

