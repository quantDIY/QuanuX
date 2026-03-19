package cmd

import (
	"log"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
	"github.com/QuanuX/qxctl/internal/tui"
)

var dashboardCmd = &cobra.Command{
	Use:   "dashboard",
	Short: "Interactive TUI for Real-time QuanuX Metrics",
	Run: func(cmd *cobra.Command, args []string) {
		cmd.Help()
	},
}

var dashboardStartCmd = &cobra.Command{
	Use:   "start",
	Short: "Launches the BubbleTea Elm Engine",
	RunE: func(cmd *cobra.Command, args []string) error {
		monitor := viper.GetBool("dashboard.dashboard.start.monitor")
		err := tui.StartDashboard(cmd.Context(), monitor)
		if err != nil {
			log.Fatalf("Fatal terminal crash: %v", err)
		}
		return nil
	},
}

func init() {
	rootCmd.AddCommand(dashboardCmd)
	dashboardCmd.AddCommand(dashboardStartCmd)
	dashboardStartCmd.Flags().Bool("monitor", false, "Enable live NATS JetStream monitoring mode")
	viper.BindPFlag("dashboard.dashboard.start.monitor", dashboardStartCmd.Flags().Lookup("monitor"))
}
