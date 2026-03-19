package cmd

import (
	"fmt"
	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var crucibleCmd = &cobra.Command{
	Use:   "crucible",
	Short: "Manage QuanuX Crucible (Backtesting Studio)",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("crucible invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var crucibleReportCmd = &cobra.Command{
	Use:   "report [strategy]",
	Short: "Retrieve instantaneous cache-aligned Backtest Metrics directly from the C++ Engine via Cython",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("report [strategy] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var crucibleReportAdvancedCmd = &cobra.Command{
	Use:   "report-advanced [strategy]",
	Short: "Retrieve Phase 5 Deep Statistical Analysis directly from the C++ Engine via Cython",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("report-advanced [strategy] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var crucibleStartCmd = &cobra.Command{
	Use:   "start [strategy]",
	Short: "Start an isolated QuanuX Crucible backtest run",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("start [strategy] invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var crucibleStatusCmd = &cobra.Command{
	Use:   "status",
	Short: "Check the status of the Crucible Engine",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("status invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

var crucibleStopCmd = &cobra.Command{
	Use:   "stop",
	Short: "Stop the currently running Crucible backtest",
	Run: func(cmd *cobra.Command, args []string) {
        fmt.Println("stop invoked.")
        fmt.Printf("Viper State: %+v\n", viper.AllSettings())
	},
}

func init() {
	rootCmd.AddCommand(crucibleCmd)
	crucibleCmd.AddCommand(crucibleReportCmd)
	crucibleReportCmd.Flags().String("version", "1.0.0", "The version of the strategy.")
	viper.BindPFlag("crucible.crucible.report.version", crucibleReportCmd.Flags().Lookup("version"))
	crucibleCmd.AddCommand(crucibleReportAdvancedCmd)
	crucibleReportAdvancedCmd.Flags().String("version", "1.0.0", "The version of the strategy.")
	viper.BindPFlag("crucible.crucible.report_advanced.version", crucibleReportAdvancedCmd.Flags().Lookup("version"))
	crucibleReportAdvancedCmd.Flags().Int("mc-iterations", 1000, "Number of Monte Carlo resampling iterations.")
	viper.BindPFlag("crucible.crucible.report_advanced.mc_iterations", crucibleReportAdvancedCmd.Flags().Lookup("mc-iterations"))
	crucibleCmd.AddCommand(crucibleStartCmd)
	crucibleStartCmd.Flags().String("version", "1.0.0", "The version of the strategy.")
	viper.BindPFlag("crucible.crucible.start.version", crucibleStartCmd.Flags().Lookup("version"))
	crucibleCmd.AddCommand(crucibleStatusCmd)
	crucibleCmd.AddCommand(crucibleStopCmd)
}

