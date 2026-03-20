package cmd

import (
	"github.com/QuanuX/qxctl/pkg/bridge"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var bridgeCmd = &cobra.Command{
	Use:   "bridge",
	Short: "Manage the SignalR Bridge",
}

var bridgeStartCmd = &cobra.Command{
	Use:   "start",
	Short: "Start the SignalR bridge process",
	RunE: func(cmd *cobra.Command, args []string) error {
		runtime := viper.GetString("bridge.bridge.start.runtime")
		port := viper.GetInt("bridge.bridge.start.port")
		return bridge.Start(cmd.Context(), runtime, port)
	},
}

var bridgeStopCmd = &cobra.Command{
	Use:   "stop",
	Short: "Stop the SignalR bridge (placeholder)",
}

func init() {
	rootCmd.AddCommand(bridgeCmd)
	bridgeCmd.AddCommand(bridgeStartCmd)
	bridgeStartCmd.Flags().StringP("runtime", "r", "flask", "Runtime to use: 'flask' or 'node'.")
	viper.BindPFlag("bridge.bridge.start.runtime", bridgeStartCmd.Flags().Lookup("runtime"))
	bridgeStartCmd.Flags().IntP("port", "p", 8077, "Port to bind the bridge to.")
	viper.BindPFlag("bridge.bridge.start.port", bridgeStartCmd.Flags().Lookup("port"))
	bridgeCmd.AddCommand(bridgeStopCmd)
}
